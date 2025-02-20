#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <omp.h>


omp_lock_t global_mtx;

//tablica mutexów dla każdego kafelka - korytarza
struct LabirynthTile {
    char tile = '0';
    int color[3] = { 0, 0, 0 };
    omp_lock_t tileMutex;
    void setTileMutex(int someTid) {

        omp_set_lock(&tileMutex);
        tile = (someTid > 9) ? 'A' + (someTid - 10) % 26 : '0' + someTid;
        color[0] = 255 * ((someTid / 1) % 5) / 4;
        color[1] = 255 * ((someTid / 5) % 5) / 4;
        color[2] = 255 * ((someTid / 25) % 5) / 4;
        omp_unset_lock(&tileMutex);
    }
};

struct ThreadLabirynth {
    LabirynthTile** labirynth = nullptr;
    const std::pair<int, int> directions[4] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
    size_t rows = 0;
    size_t cols = 0;
    static int tid;

    ThreadLabirynth() = delete;
    ThreadLabirynth(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(file, line)) { lines.push_back(line); }
        file.close();
        rows = lines.size();
        cols = lines[0].size();

        labirynth = new LabirynthTile * [rows];
        for (int i = 0; i < rows; i++) { labirynth[i] = new LabirynthTile[cols]; }

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) { labirynth[i][j].tile = lines[i][j]; }
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) { omp_init_lock(&labirynth[i][j].tileMutex); }
        }

    }
    ~ThreadLabirynth() {

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) { omp_destroy_lock(&labirynth[i][j].tileMutex); }
        }

        for (int i = 0; i < rows; i++) { delete[] labirynth[i]; }
        delete[] labirynth;
        labirynth = nullptr;
    }

    void display() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) { std::cout << labirynth[i][j].tile; }
            std::cout << '\n';
        }
    }
    void exportToPPM(int scale, std::string filename) {
        std::ofstream file(filename);
        file << "P3\n" << cols * scale << " " << rows * scale << "\n255\n";
        for (int i = 0; i < rows; i++) {
            for (int s = 0; s < scale; s++) { // wiersze
                for (int j = 0; j < cols; j++) {
                    for (int t = 0; t < scale; t++) { // kolumny
                        file << labirynth[i][j].color[0] << " " << labirynth[i][j].color[1] << " " << labirynth[i][j].color[2] << " ";
                    }
                }
                file << '\n';
            }
        }
        file.close();
    }

    bool checkIfInBounds(int x, int y) { return !(x < 0 || x >= rows || y < 0 || y >= cols); }
    bool checkIfAvailable(int x, int y, char free) { return labirynth[x][y].tile == free; }
    void threadRoutine(int myTid, int x, int y) {
        int moveIndex;
        do {
            bool dir = 1;
            moveIndex = -1;
            //sprawdzanie legalnosci i dostepnosci
            for (int i = 0; i < 4; i++) {
                if (checkIfInBounds(x + directions[i].first, y + directions[i].second)) {
                    omp_set_lock(&labirynth[x + directions[i].first][y + directions[i].second].tileMutex); //to jest chyba potrzebne? w sensie, odczytuję cos co może być zmieniane
                    bool isAvailable = checkIfAvailable(x + directions[i].first, y + directions[i].second, '0');
                    omp_unset_lock(&labirynth[x + directions[i].first][y + directions[i].second].tileMutex);
                    if (dir && isAvailable) {
                        labirynth[x + directions[i].first][y + directions[i].second].setTileMutex(myTid);
                        moveIndex = i;
                        dir = 0;
                    }
                    else if (isAvailable) {
                        omp_set_lock(&global_mtx);
                        tid = tid + 1;
                        labirynth[x + directions[i].first][y + directions[i].second].setTileMutex(tid);
                        #pragma omp task
                        {
                            threadRoutine(tid, x + directions[i].first, y + directions[i].second);
                        }
                        omp_unset_lock(&global_mtx);
                    }
                }

            }
            if (moveIndex >= 0) {
                x += directions[moveIndex].first;
                y += directions[moveIndex].second;
            }
        } while (moveIndex >= 0);

        #pragma omp taskwait

    }

    void traverseLabirynth(int x, int y) {

    #pragma omp parallel num_threads(8)
        {
            #pragma omp single
            {
                labirynth[x][y].setTileMutex(tid);
                threadRoutine(tid, x, y);
            }
        }
    }

    bool destroyWall() {}


};
int ThreadLabirynth::tid = 1;

int main()
{
    omp_init_lock(&global_mtx);

    std::string name = "labirynt40x40.ppm";
    ThreadLabirynth* test = new ThreadLabirynth("labirynt40x40.txt");
    test->display();
    auto start = omp_get_wtime();

    test->traverseLabirynth(0, 0);

    auto end = omp_get_wtime();
    std::cout << end - start << "s\n";
    test->display();
    test->exportToPPM(10, name);
    delete test;
    return 0;
}

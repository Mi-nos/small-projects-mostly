#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <thread>


std::mutex global_mtx;
//tablica mutexów dla każdego kafelka - korytarza
struct LabirynthTile {
    char tile = '0';
    int color[3] = {0, 0, 0};
    std::mutex tileMutex;
    void setTileMutex(int someTid) {
        tileMutex.lock();
		tile = (someTid > 9) ? 'A' + (someTid - 10) % 26 : '0' + someTid;
        color[0] = 255 * ((someTid / 1) % 5) / 4;   
        color[1] = 255 * ((someTid / 5) % 5) / 4;    
        color[2] = 255 * ((someTid / 25) % 5) / 4;   
		tileMutex.unlock();
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
	}
	~ThreadLabirynth() {
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
    //jedna funkcja do której mam wątpliwosci
    bool checkIfAvailable(int x, int y, char free) { return labirynth[x][y].tile == free; }
    void threadRoutine(int myTid, int x, int y) {
        //nie musi się wpisywać bo już tam jest, zadbał o to poprzedni wątek
		std::vector<std::pair<std::thread, int>> threadVector;
        int moveIndex;
        do {
            bool dir = 1;
            moveIndex = -1;
			//sprawdzanie legalnosci i dostepnosci
            for (int i = 0; i < 4; i++) {
                if (checkIfInBounds(x + directions[i].first, y + directions[i].second)) {
					labirynth[x + directions[i].first][y + directions[i].second].tileMutex.lock(); //to jest chyba potrzebne? w sensie, odczytuję cos co może być zmieniane
					bool isAvailable = checkIfAvailable(x + directions[i].first, y + directions[i].second, '0');
					labirynth[x + directions[i].first][y + directions[i].second].tileMutex.unlock();
                    if (dir && isAvailable) {
                        labirynth[x + directions[i].first][y + directions[i].second].setTileMutex(myTid);
                        moveIndex = i;
                        dir = 0;
                    }
                    else if (isAvailable) {
                        global_mtx.lock();
                        tid = tid + 1;
						labirynth[x + directions[i].first][y + directions[i].second].setTileMutex(tid);
                        threadVector.push_back(std::make_pair(std::thread(&ThreadLabirynth::threadRoutine, this, tid, x + directions[i].first, y + directions[i].second), tid));
                        global_mtx.unlock();
                    }
                }

            }
			if (moveIndex >= 0) {
				x += directions[moveIndex].first;
				y += directions[moveIndex].second;
			}
		} while (moveIndex >= 0);
        //teraz tylko wypisujemy dzieci i czekamy na ich zakończenie

        for (auto& thread : threadVector) {
            if (thread.first.joinable()) { thread.first.join(); }
        }
		global_mtx.lock();
        std::cout << "Children of thread: " << myTid << std::endl;
        for (auto& thread : threadVector) {
            std::cout << thread.second << ", ";
        }
        std::cout << std::endl;
		global_mtx.unlock();
    }

    void traverseLabirynth(int x, int y) {
		labirynth[x][y].setTileMutex(tid);
        std::thread mainThread(&ThreadLabirynth::threadRoutine, this, tid, x, y);
        mainThread.join();
    }

    bool destroyWall() {}


};
int ThreadLabirynth::tid = 1;

int main()
{
	std::string name = "labirynt20x20.ppm";
	ThreadLabirynth *test = new ThreadLabirynth("labirynt20x20.txt");
    test->display();
    auto start{ std::chrono::steady_clock::now() };
	test->traverseLabirynth(0, 0);
    auto end{ std::chrono::steady_clock::now() };
    std::chrono::duration<double> elapsed_seconds{ end - start };
    std::cout << elapsed_seconds.count() << "s\n";
    test->display();
	test->exportToPPM(10, name);
    delete test;
	return 0;
}

//void threadRoutineOLD(int myTid, int x, int y) {
//    char mySymbol = (myTid > 9) ? 'A' + myTid - 10 : '0' + myTid;
//
//    labirynth[x][y].setTileMutex(mySymbol);
//    std::vector<std::pair<std::thread, int>> threadVector;
//
//    int moveIndex = -1;
//    do {
//        bool dir = 1;
//        moveIndex = -1;
//        bool isLegal[] = { checkIfInBounds(x - 1, y), checkIfInBounds(x, y + 1), checkIfInBounds(x + 1, y), checkIfInBounds(x, y - 1) };
//        bool isFree[] = { 0, 0, 0, 0 };
//        //tutaj są zapisane legalne i dostępne ruchy
//        for (int i = 0; i < 4; i++) {
//            if (isLegal[i]) { isFree[i] = checkIfAvailable(x + directions[i].first, y + directions[i].second, '0'); }
//        }
//
//        for (int i = 0; i < 4; i++) {
//            if (dir && isFree[i]) {
//                labirynth[x + directions[i].first][y + directions[i].second].setTileMutex(mySymbol);
//                moveIndex = i;
//                dir = 0;
//            }
//            else if (isFree[i]) {
//                global_mtx.lock();
//                tid = tid + 1;
//                threadVector.push_back(std::make_pair(std::thread(&ThreadLabirynth::threadRoutineOLD, this, tid, x + directions[i].first, y + directions[i].second), tid));
//                global_mtx.unlock();
//            }
//        }
//        if (moveIndex >= 0) {
//            x += directions[moveIndex].first;
//            y += directions[moveIndex].second;
//        }
//
//    } while (moveIndex >= 0);
//    //teraz tylko wypisujemy dzieci i czekamy na ich zakończenie
//    global_mtx.lock();
//    std::cout << "Children of thread: " << myTid << std::endl;
//    for (auto& thread : threadVector) {
//        std::cout << thread.second << ", ";
//    }
//    std::cout << std::endl;
//    global_mtx.unlock();
//
//    for (auto& thread : threadVector) {
//        if (thread.first.joinable()) { thread.first.join(); }
//    }
//}
#include <iostream>
#include <omp.h>
#include <cmath>
#include <string>
#include <fstream>
#include <iomanip>
//https://stackoverflow.com/questions/61257209/openmp-behavior-nested-multithreading
namespace {
    const int N = 16384;
    int NUM_TESTS = 5;
    unsigned char color[N][N][3];
    const int MaxColorComponentValue = 255;
}

int ulam_get_map(int x, int y, long n) {
    x -= (n - 1) / 2;
    y -= n / 2;
    int mx = abs(x), my = abs(y);
    int l = 2 * std::max(mx, my);
    int d = y >= x ? l * 3 + x + y : l - x - y;
    return pow(l - 1, 2) + d;
}

int isprime(int n) {
    if (n <= 1) return 0;
    for (int p = 2; p * p <= n; p++)
        if (n % p == 0) return 0;
    return 1;
}
//załóżmy rozmiary macierzy niewymagające poprawek, tj. 16x16, 32x32, 64x64 etc
void processQuartersNxN(int N, int block_size) {
    #pragma omp parallel for collapse(2) num_threads(4) //Podział na 4 ćwiartki, każda dostaje start i koniec
    for (int qx = 0; qx < 2; ++qx) {    
        for (int qy = 0; qy < 2; ++qy) {
            // Zakresy dla tej ćwiartki
            int x_start = qx * (N / 2);
            int x_end = x_start + (N / 2);
            int y_start = qy * (N / 2);
            int y_end = y_start + (N / 2);

            // Podział ćwiartki na bloki o rozmiarze block_size
            #pragma omp parallel for collapse(2) num_threads(block_size) //podział na bloki, każdy dostaje też start i koniec
            for (int bx = x_start; bx < x_end; bx += block_size) {
                for (int by = y_start; by < y_end; by += block_size) {
                    //przetwarzanie każdego bloku
                    for (int x = bx; x < bx + block_size; ++x) {
                        for (int y = by; y < by + block_size; ++y) {
                            auto result = isprime(ulam_get_map(x, y, N));
                            if (result) {
                                color[x][y][0] = 0;
                                color[x][y][1] = 0;
                                color[x][y][2] = 0;
                            }
                            else {
                                color[x][y][0] = 255;
                                color[x][y][1] = 255;
                                color[x][y][2] = 255;
                            }
                        }
                    }
                }
            }
        }
    }
}

void process_rows(int N, int chunk_size) {
    #pragma omp parallel for num_threads(8) schedule(static, chunk_size)
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) { 
            auto result = isprime(ulam_get_map(x, y, N)); 
            if (result) {
                color[x][y][0] = 0;
                color[x][y][1] = 0;
                color[x][y][2] = 0;
            }
            else {
                color[x][y][0] = 255;
                color[x][y][1] = 255;
                color[x][y][2] = 255;
            }

        }
    }
}

int main() {

    int sizes[] = {4096, 8192, 16384};
    int chunks[] = {1, 10, 100, 1000};
    std::ofstream file;
    file.open("ulam_wyniki.csv", std::ofstream::out | std::ofstream::app);

    double start, end;
    omp_set_nested(1);

    for (auto& size : sizes) {
        FILE* fp;
        std::string name = std::to_string(size) + "_ulam.ppm";
        const char* filename = name.c_str();
        fp = fopen(filename, "wb");
        fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", "# ", N, N, MaxColorComponentValue);


        file << size << "x" << size << "\n";
        std::cout << "Rozmiar to: " << size << "x" << size << std::endl;

        std::cout << "Podzial wierszowy:" << std::endl;
        for (auto& chunk : chunks) {
            file << "Chunk size: " << chunk << "\n";
            std::cout << "Bloki o rozmiarze: " << chunk << std::endl;
            for (int i = 0; i < NUM_TESTS; i++) {
                start = omp_get_wtime();
                process_rows(size, chunk);
                fwrite(color, sizeof(unsigned char), size * size * 3, fp);
                end = omp_get_wtime();
                std::cout << (end - start) << "s\n";
                file << std::fixed << std::setprecision(3) << (end - start) << "\n";
            }
        }
        file << "bloki 2x2" << "\n";
        std::cout << "Cwiartki bloki 2x2:" << std::endl;
        for (int i = 0; i < NUM_TESTS; i++) {
            start = omp_get_wtime();
            processQuartersNxN(size, 4);
            fwrite(color, sizeof(unsigned char), size * size * 3, fp);
            end = omp_get_wtime();
            std::cout << (end - start) << "s\n";
            file << std::fixed << std::setprecision(3) << (end - start) << "\n";
        }
        file << "bloki 4x4" << "\n";
        std::cout << "Cwiartki bloki 4x4:" << std::endl;
        for (int i = 0; i < NUM_TESTS; i++) {
            start = omp_get_wtime();
            processQuartersNxN(size, 16);
            fwrite(color, sizeof(unsigned char), size * size * 3, fp);
            end = omp_get_wtime();
            std::cout << (end - start) << "s\n";
            file << std::fixed << std::setprecision(3) << (end - start) << "\n";
        }
        fclose(fp);
    }
    
    file.close();
    return 0;
}
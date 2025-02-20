#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>


namespace {
    //rozmiary powinny być zgodne z kafelkami, czyli 1024, 2048, 4096 etc
    int N = 4096;
    int MAX_NUMBER_OF_THREADS = 8;
    int NUMBER_OF_TESTS = 5;
    int MAX_BLOCK_SIZE = 256;

    std::vector<std::vector<int>> A(N, std::vector<int>(N, 0));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 0));
    std::vector<std::vector<int>> B_T(N, std::vector<int>(N, 0));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));


}
void zeroMatrix(std::vector<std::vector<int>>& matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) { matrix[i][j] = 0; }
    }
}
void displayMatrix(std::vector<std::vector<int>>& matrix) {
    if (matrix.empty()) { 
        std::cout << "nice tu nie ma " << std::endl; 
        return;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << matrix[i][j] << "___";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}
void fillMatrix(std::vector<std::vector<int>>& matrix, int start) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) { 
            matrix[i][j] = start;
            start++;
        }
    }
}
void identityMatrix(std::vector<std::vector<int>>& matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == j) { matrix[i][j] = 1; }
            else { matrix[i][j] = 0; }
        }
    }
}

void openMP(int NUMBER_OF_THREADS) {
#pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        #pragma omp for schedule(static, 1)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) { C[i][j] += A[i][k] * B[k][j]; }
            }
        }

    }
}
void openMP_transposed(int NUMBER_OF_THREADS) {
    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        #pragma omp for schedule(static, 1)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) { B_T[i][j] = B[j][i]; }
        }

        #pragma omp for schedule(static, 1)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) { C[i][j] += A[i][k] * B_T[j][k]; }
            }
        }

    }
}
void openMP_block_tiling(int NUMBER_OF_THREADS, int BLOCK_SIZE) {
    #pragma omp parallel num_threads(NUMBER_OF_THREADS) 
    {
        //transpozycja
        //#pragma omp for schedule(static, 1)
        //for (int i = 0; i < N; i++) {
        //    for (int j = 0; j < N; j++) {
        //        B_T[i][j] = B[j][i];
        //    }
        //}
        
        //block tiling
        #pragma omp for schedule(static, 1) 
        for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
            for (int jj = 0; jj < N; jj += BLOCK_SIZE) {         

                for (int kk = 0; kk < N; kk += BLOCK_SIZE) {
                    for (int i = ii; i < ii + BLOCK_SIZE; i++) {
                        for (int k = kk; k < kk + BLOCK_SIZE; k++) {
                            for (int j = jj; j < jj + BLOCK_SIZE; j++) {
                                C[i][j] += A[i][k] * B[k][j];
                            }
                        }
                    }
                }
            }
        }


    }
}
int main(int argc, char* argv[]) {
    if (argc == 5) {
        N = std::atoi(argv[1]);
        MAX_NUMBER_OF_THREADS = std::atoi(argv[2]);
        NUMBER_OF_TESTS = std::atoi(argv[3]);
        MAX_BLOCK_SIZE = std::atoi(argv[4]);
    }
    std::ofstream file;
    file.open("C:/Users/Maks/source/repos/priw4/priw4/priw4wyniki.csv", std::ofstream::out | std::ofstream::app);
    std::cout << "Rozmiar macierzy: " << N << " x " << N << std::endl;
    file << N << "x" << N << "\n";
    auto start = omp_get_wtime();
    auto end = omp_get_wtime();
    auto exec_time = end - start;

    fillMatrix(A, 1);
    identityMatrix(B);

    //default
    std::cout << "OpenMP, default" << std::endl;
    file << "OpenMP default" << "\n";
    for (int m = 4; m <= MAX_NUMBER_OF_THREADS; m *= 2) {
        std::cout << "Threads: " << m << std::endl;
        file << "Threads: " << m << "\n"; 
        for (int n = 0; n < NUMBER_OF_TESTS; n++) {
            zeroMatrix(C);
            start = omp_get_wtime();
            openMP(m);
            end = omp_get_wtime();
            exec_time = end - start;
            std::cout << exec_time << std::endl;
            file << exec_time << ",";
        }
        file << "\n";
    }

    //default z transpozycją
    std::cout << "/////////////////////////" << std::endl;
    
    std::cout << "OpenMP, transposed" << std::endl;
    file << "OpenMP transposed" << "\n";
    for (int m = 2; m <= MAX_NUMBER_OF_THREADS; m *= 2) {
        std::cout << "Threads: " << m << std::endl;
        file << "Threads: " << m << "\n";
        for (int n = 0; n < NUMBER_OF_TESTS; n++) {
            zeroMatrix(C);
            zeroMatrix(B_T);
            start = omp_get_wtime();
            openMP_transposed(m);
            end = omp_get_wtime();
            exec_time = end - start;
            std::cout << exec_time << std::endl;
            file << exec_time << ",";
        }
        file << "\n";
    }
    
    
    std::cout << "/////////////////////////" << std::endl;

    //block - tiling
    std::cout << "OpenMP block - tiling" << std::endl;
    file << "OpenMP block - tiling" << "\n";
    for (int m = 2; m <= MAX_NUMBER_OF_THREADS; m *= 2) {
        std::cout << "Threads: " << m << std::endl;
        file << "Threads: " << m << "\n";
        for (int block = 2; block <= MAX_BLOCK_SIZE; block *= 2) {
            std::cout << "Block size: " << block << std::endl;
            file << "Block size: " << block << ",";
            for (int n = 0; n < NUMBER_OF_TESTS; n++) {
                zeroMatrix(C);
                start = omp_get_wtime();
                openMP_block_tiling(m, block);
                end = omp_get_wtime();
                exec_time = end - start;
                std::cout << exec_time << std::endl;
                file << exec_time << ",";
            }
            file << "\n";
        }
        file << "\n";
    }

    file.close();
    return 0;
}
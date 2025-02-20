#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <omp.h>

namespace {
    int MAX_THREADS = 8;
    int NUM_TESTS = 5;
    int MAX_CHUNK_SIZE = 64;

    std::vector<double> thread_times;
    std::vector<int> thread_iters;
 

    const int iXmax = 1600;
    const int iYmax = iXmax;

    const double CxMin = -2.5;
    const double CxMax = 1.5;
    const double CyMin = -2.0;
    const double CyMax = 2.0;

    const int MaxColorComponentValue = 255;
    const int IterationMax = 200;
    const double EscapeRadius = 2;

    const double PixelWidth = (CxMax - CxMin) / iXmax;
    const double PixelHeight = (CyMax - CyMin) / iYmax;

    unsigned char color[iYmax][iXmax][3];
    const double ER2 = EscapeRadius * EscapeRadius;
}

void mandelbrot_OpenMP(int threads, int chunk_size) {
    
    double Zx, Zy;
    double Zx2, Zy2;
    double Cx, Cy;
    int iX, iY;
    int Iteration;
    long long fullIterations = 0;
    #pragma omp parallel  private(fullIterations, Cx, Cy, Zx, Zy, Zx2, Zy2, Iteration, iX, iY) num_threads(threads) 
    {
        auto start_task = omp_get_wtime();
        fullIterations = 0;
        #pragma omp for schedule(guided, chunk_size) //chunk-size to drugi argument, ilosc pętli do przydzielenia wątkowi
        for (iY = 0; iY < iYmax; iY++) {
            //std::cout << "Thread: " << omp_get_thread_num() << "robi iY od: " << iY << std::endl;

            Cy = CyMin + iY * PixelHeight;
            if (fabs(Cy) < PixelHeight / 2) Cy = 0.0; /* Main antenna */
            for (iX = 0; iX < iXmax; iX++) {
                Cx = CxMin + iX * PixelWidth;
                /* initial value of orbit = critical point Z= 0 */
                Zx = 0.0;
                Zy = 0.0;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
                /* */
                for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
                {
                    Zy = 2 * Zx * Zy + Cy;
                    Zx = Zx2 - Zy2 + Cx;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
                };
                /* compute  pixel color (24 bit = 3 bytes) */
                if (Iteration == IterationMax)
                { /*  interior of Mandelbrot set = black */
                    color[iY][iX][0] = 0;
                    color[iY][iX][1] = 0;
                    color[iY][iX][2] = 0;
                    fullIterations++;
                }
                else
                { /* exterior of Mandelbrot set = white */
                    int thread_id = omp_get_thread_num();
                    color[iY][iX][0] = (thread_id * 50) % 256; // Red component
                    color[iY][iX][1] = (thread_id * 80 + 50) % 256; // Green component
                    color[iY][iX][2] = (thread_id * 100 + 100) % 256; // Blue component
                };
            }
            
        }
        auto end_task = omp_get_wtime();
        #pragma omp critical 
        {
            thread_times.push_back(end_task - start_task);
            thread_iters.push_back(fullIterations);
        }

    }
}

int main(int argc, char* argv[])
{
    if (argv[1]) { MAX_THREADS = std::atoi(argv[1]); }
    if (argv[2]) { MAX_CHUNK_SIZE = std::atoi(argv[2]); }
    if (argv[3]) { NUM_TESTS = std::atoi(argv[3]); }
    std::ofstream file;
    file.open("C:/Users/Maks/source/repos/priw5/priw5/priw5wyniki.csv", std::ofstream::out | std::ofstream::app);
    file << "\n";
    std::cout << "Rozmiar obrazka: " << iYmax << " x " << iXmax << std::endl;
    file << iYmax << "x" << iXmax << "\n";

    for (int current_threads = 2; current_threads <= MAX_THREADS; current_threads *= 2) {
        std::cout << "Threads : " << current_threads << std::endl;
        file << "Threads: " << current_threads << "\n";
        for (int current_chunk_size = 1; current_chunk_size <= MAX_CHUNK_SIZE; current_chunk_size *= 2) {
            std::cout << "Chunk size : " << current_chunk_size << std::endl;
            file << "Chunk size: " << current_chunk_size << "," << "Idle-time" << "," << "Iteracje" << "\n";
            for (int i = 0; i < NUM_TESTS; i++) {
                FILE* fp;
                std::string name = std::to_string(current_threads) + "_" + std::to_string(iXmax) + "_rownolegle_guided.ppm";
                const char* filename = name.c_str();
                fp = fopen(filename, "wb");
                fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", "# ", iXmax, iYmax, MaxColorComponentValue);

                auto start = omp_get_wtime();
                mandelbrot_OpenMP(current_threads, current_chunk_size);
                fwrite(color, sizeof(unsigned char), iYmax * iXmax * 3, fp);
                auto end = omp_get_wtime();
                auto exec_time = end - start;
                std::cout << exec_time << std::endl;
                auto minmax_times = std::minmax_element(thread_times.begin(), thread_times.end());
                auto minmax_iters = std::minmax_element(thread_iters.begin(), thread_iters.end());
                std::cout << "Idle-time: " << (*minmax_times.second) - (*minmax_times.first) << std::endl;
                std::cout << "Roznica w iteracjach: " << (*minmax_iters.second) - (*minmax_iters.first) << std::endl;

                for (auto& iters : thread_iters) { std::cout << iters << std::endl; }
                //for (auto& iters : thread_times) { std::cout << iters << std::endl; }
                file << exec_time << "," << (*minmax_times.second) - (*minmax_times.first) << "," << (*minmax_iters.second) - (*minmax_iters.first);
                thread_iters.clear();
                thread_times.clear();
                fclose(fp);
                file << "\n";
            }
        }
        file << "\n";
        
    }
    
    
    
    
    
    
    
    
    return 0;
}
#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <tbb/tbb.h>
#include <iomanip>

namespace {
    const double CxMin = -2.5;
    const double CxMax = 1.5;
    const double CyMin = -2.0;
    const double CyMax = 2.0;

    const int MaxColorComponentValue = 255;
    const int IterationMax = 200;
    const double EscapeRadius = 2;

    double PixelWidth = (CxMax - CxMin) / 1;
    double PixelHeight = (CyMax - CyMin) / 1;
    const double ER2 = EscapeRadius * EscapeRadius;
}

unsigned char*** createColorArray(int width, int height) {
    unsigned char*** colorArray = new unsigned char** [height];

    for (int i = 0; i < height; ++i) {
        colorArray[i] = new unsigned char* [width];

        for (int j = 0; j < width; ++j) {
            colorArray[i][j] = new unsigned char[3];

            for (int k = 0; k < 3; ++k) {
                colorArray[i][j][k] = 0;
            }
        }
    }
    return colorArray;
}

void saveImageToPPM(unsigned char*** colorArray, int width, int height, const std::string& outputPath) {
    std::ofstream file(outputPath, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            //musia?em castowa? bo write szuka const char* a mamy unsigned char*
            file.write(reinterpret_cast<char*>(colorArray[y][x]), 3);
        }
    }

    file.close();
    std::cout << "Obraz zapisany jako " << outputPath << "\n";
}

void freeImageColors(unsigned char*** colorArray, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            delete[] colorArray[y][x];
        }
        delete[] colorArray[y];
    }
    delete[] colorArray;
}

void mandelbrot_TD(unsigned char*** colorArray, int width, int height, int grain_size) {

    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, height, grain_size, 0, width, grain_size),
        [&](const tbb::blocked_range2d<int>& range) {
            double Zx, Zy;
            double Zx2, Zy2;
            double Cx, Cy;
            int Iteration;
            for (int y = range.rows().begin(); y < range.rows().end(); ++y) {
                Cy = CyMin + y * PixelHeight;
                if (fabs(Cy) < PixelHeight / 2) Cy = 0.0; /* Main antenna */
                for (int x = range.cols().begin(); x < range.cols().end(); ++x) {
                    Cx = CxMin + x * PixelWidth;
                    /* initial value of orbit = critical point Z= 0 */
                    Zx = 0.0;
                    Zy = 0.0;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
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
                        colorArray[y][x][0] = 0;
                        colorArray[y][x][1] = 0;
                        colorArray[y][x][2] = 0;
                    }
                    else
                    { /* exterior of Mandelbrot set = white */
                        //std::size_t thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
                        colorArray[y][x][0] = 255; // Red component
                        colorArray[y][x][1] = 255; // Green component
                        colorArray[y][x][2] = 255; // Blue component
                    };
                }
            }
        });
}


int main(int argc, char* argv[]) {


    int grains[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    bool timed = true;

    int width = 3200;
    int height = width;

    PixelWidth = (CxMax - CxMin) / width;
    PixelHeight = (CyMax - CyMin) / height;

    unsigned char*** colorArray = createColorArray(width, height);

    if (timed) {
        std::ofstream file;
        file.open("mandelbrot_wyniki_NOWE.csv", std::ofstream::out | std::ofstream::app);
        file << width << "x" << height << "\n";
        for (auto& grain : grains) {
            file << "Ziarno: " << grain << "\n";
            std::cout << "Ziarno: " << grain << "\n";
            for (int i = 0; i < 5; i++) {
                tbb::tick_count t1 = tbb::tick_count::now();
                mandelbrot_TD(colorArray, width, height, grain);
                tbb::tick_count t2 = tbb::tick_count::now();
                file << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << "\n";
                std::cout << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << std::endl;
            }
        }
        file.close();
    }

    else {
        mandelbrot_TD(colorArray, width, height, 100);
    }

    std::string outputPath = std::to_string(width) + "x" + std::to_string(height) + "_mandelbrot.ppm";

    saveImageToPPM(colorArray, width, height, outputPath);
    freeImageColors(colorArray, width, height);

    return 0;
}
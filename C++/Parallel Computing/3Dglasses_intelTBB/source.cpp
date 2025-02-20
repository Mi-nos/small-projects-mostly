#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <tbb/tbb.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
    std::vector<std::vector<int>> mean3x3{ {1, 1, 1}, {1, 1, 1}, {1, 1, 1} };
    std::vector<std::vector<int>> LP2_3x3{ {1, 1, 1}, {1, 4, 1}, {1, 1, 1} };
    std::vector<std::vector<int>> gauss3x3{ {1, 2, 1}, {2, 4, 2}, {1, 2, 1} };
    std::vector<std::vector<int>> mean_removal3x3{ {-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1} };
    std::vector<std::vector<int>> horizontal3x3{ {0, 0, 0}, {-1, 1, 0}, {0, 0, 0} };
    std::vector<std::vector<int>> LAPL3{ {1, -2, 1}, {-2, 4, -2}, {1, -2, 1} };


    std::vector<std::vector<int>> gauss4_5x5{ {1, 4, 7, 4, 1}, {4, 16, 26, 16, 4}, {7, 26, 41, 26, 7}, {4, 16, 26, 16, 4}, {1, 4, 7, 4, 1} };

    std::vector<std::vector<int>> mean7x7{ {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1} };
}

//stb_image
unsigned char*** loadImageColors(const std::string& filepath, int& width, int& height) {
    int channels; // Liczba kanałów obrazu (np. 3 dla RGB)
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 3);
    unsigned char*** colorArray = new unsigned char** [height];
    for (int y = 0; y < height; ++y) {
        colorArray[y] = new unsigned char* [width];
        for (int x = 0; x < width; ++x) {
            colorArray[y][x] = new unsigned char[3];
            // Kopiowanie warto?ci RGB z surowych danych
            int index = (y * width + x) * 3;
            colorArray[y][x][0] = data[index];
            colorArray[y][x][1] = data[index + 1];
            colorArray[y][x][2] = data[index + 2];
        }
    }

    stbi_image_free(data); // Zwolnienie surowych danych
    return colorArray;
}

// Funkcja zapisujaca obraz w formacie PPM
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

// Funkcja zwalniająca pamięć
void freeImageColors(unsigned char*** colorArray, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            delete[] colorArray[y][x];
        }
        delete[] colorArray[y];
    }
    delete[] colorArray;
}

void applyFilter3D(unsigned char*** colors, unsigned char*** output, int width, int height, int offset) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < 3; ++c) {
                switch (c) {
                    // Kanał red: przesunięcie w lewo
                case 0:
                    if (x - offset >= 0) {
                        output[y][x][c] = colors[y][x - offset][c];
                    }
                    else {
                        output[y][x][c] = 0;
                    }
                    break;

                    // Kanał green: wyzerowanie
                case 1:
                    output[y][x][c] = 0;
                    break;

                    // Kanał blue: przesunięcie w prawo
                case 2:
                    if (x + offset < width) {
                        output[y][x][c] = colors[y][x + offset][c];
                    }
                    else {
                        output[y][x][c] = 0;
                    }
                    break;
                }
            }
        }
    }
}

void applyFilterParallel3D(unsigned char*** colors, unsigned char*** output, int width, int height, int grain_size, int offset) {

    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, height, grain_size, 0, width, grain_size),
        [&](const tbb::blocked_range2d<int>& range) {
            for (int y = range.rows().begin(); y < range.rows().end(); ++y) {
                for (int x = range.cols().begin(); x < range.cols().end(); ++x) {
                    for (int c = 0; c < 3; ++c) {
                        switch (c) {
                            //kanał red, robimy przesunięcie
                            case 0:
                                if (x - offset >= 0) { output[y][x][c] = colors[y][x - offset][c]; }
                                else { output[y][x][c] = 0; }
                                break;
                            //kanał green, zerujemy
                            case 1:
                                output[y][x][c] = 0;
                                break;
                            //kanał blue, robimy przesunięcie
                            case 2:
                                if (x + offset < width) { output[y][x][c] = colors[y][x + offset][c]; }
                                else { output[y][x][c] = 0; }
                                break;
                        }
                    }
                }
            }
        });
}


int main(int argc, char* argv[]) {


    int grains[] = { 1, 10, 100, 500, 1000 };
    bool timed = true;

    int width, height = 0;
    std::string filepath = "cat_superres.jpg"; // ?cie?ka do pliku wej?ciowego


    //wczytaj
    unsigned char*** colors = loadImageColors(filepath, width, height);
    unsigned char*** output = loadImageColors(filepath, width, height);
    if (timed) {
        std::ofstream file;
        file.open("filtry_wyniki.csv", std::ofstream::out | std::ofstream::app);
        file << width << "x" << height << "\n";
        for (auto& grain : grains) {
            file << "Ziarno: " << grain << "\n";
            file << "sequential, parallel" << "\n";
            std::cout << "Ziarno: " << grain << "\n";
            std::cout << "sequential, parallel" << std::endl;
            for (int i = 0; i < 5; i++) {
                tbb::tick_count t1 = tbb::tick_count::now();
                applyFilter3D(colors, output, width, height, 20);
                tbb::tick_count t2 = tbb::tick_count::now();

                file << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << ",";
                tbb::tick_count t3 = tbb::tick_count::now();
                applyFilterParallel3D(colors, output, width, height, grain, 20);
                tbb::tick_count t4 = tbb::tick_count::now();
                file << std::fixed << std::setprecision(3) << (t4 - t3).seconds() << "\n";

                std::cout << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << "    " << (t4 - t3).seconds() << std::endl;
            }
        }
        file.close();
    }

    else {
        applyFilterParallel3D(colors, output, width, height, 10, 20);
    }

    std::string outputPath = std::to_string(width) + "x" + std::to_string(height) + "_nieistotne3d.ppm";

    //zapisanie obrazu do formatu PPM
    saveImageToPPM(output, width, height, outputPath);
    //zwolnij pamięć
    freeImageColors(colors, width, height);
    freeImageColors(output, width, height);


    return 0;
}
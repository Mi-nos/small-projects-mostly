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
            // Kopiowanie wartości RGB z surowych danych
            int index = (y * width + x) * 3;
            colorArray[y][x][0] = data[index];
            colorArray[y][x][1] = data[index + 1];
            colorArray[y][x][2] = data[index + 2];
        }
    }

    stbi_image_free(data); // Zwolnienie surowych danych
    return colorArray;
}

// Funkcja zapisująca obraz w formacie PPM
void saveImageToPPM(unsigned char*** colorArray, int width, int height, const std::string& outputPath) {
    std::ofstream file(outputPath, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            //musiałem castować bo write szuka const char* a mamy unsigned char*
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

void applyFilterParallel(unsigned char*** colors, unsigned char*** output, int width, int height, std::vector<std::vector<int>>& mask, int grain_size) {
    // Liczenie sumy maski do normalizacji
    int maskSum = 0;
    for (auto& vector : mask) {
        for (auto& element : vector) {
            maskSum += element;
        }
    }

    // Padding (np. dla 3x3: 1, dla 5x5: 2)
    int padding = (mask.size() - 1) / 2;

    tbb::parallel_for(
        tbb::blocked_range2d<int>(padding, height - padding, grain_size, padding, width - padding, grain_size),
        [&](const tbb::blocked_range2d<int>& range) {
            for (int y = range.rows().begin(); y < range.rows().end(); ++y) {
                for (int x = range.cols().begin(); x < range.cols().end(); ++x) {
                    for (int c = 0; c < 3; ++c) {
                        int sum = 0;
                        for (int ky = -padding; ky <= padding; ++ky) {
                            for (int kx = -padding; kx <= padding; ++kx) {
                                int ny = y + ky;
                                int nx = x + kx;
                                sum += colors[ny][nx][c] * mask[ky + padding][kx + padding];
                            }
                        }

                        // Normalizacja
                        if (maskSum != 0) {
                            sum /= maskSum;
                        }

                        // żeby nie bylo ujemnych
                        if (sum < 0) {
                            sum = 0;
                        }
                        else if (sum > 255) {
                            sum = 255;
                        }

                      
                        output[y][x][c] = static_cast<unsigned char>(sum);
                    }
                }
            }
        });
}

void applyFilterParallelKuwahara(unsigned char*** colors, unsigned char*** output, int width, int height, std::vector<std::vector<int>>& mask, int grain_size, int region_size) {

    //tu nie potrzebujesz żadnej maski bo kuwahara działa na samych pikselach, z drugiej strony możesz przekazać mu dowolną maskę i on z niej zczyta tylko rozmiar
    //dla 3x3 padding = 1 , dla 5x5 padding = 2, dla 7x7, 3
    int padding = (mask.size() - 1) / 2;

    tbb::parallel_for(
        //omijamy piksele bez pełnego zestawu sąsiadów 
        tbb::blocked_range2d<int>(padding, height - padding, grain_size, padding, width - padding, grain_size),
        [&](const tbb::blocked_range2d<int>& range) {   
            for (int y = range.rows().begin(); y < range.rows().end(); ++y) {
                for (int x = range.cols().begin(); x < range.cols().end(); ++x) {
                    for (int c = 0; c < 3; ++c) {

                        double means[4] = { 0 };
                        double variances[4] = { 0 };
                        //kuwahara dla 3x3 ma obszary po 2x2, dla 5x5, 3x3, dla 7x7, 4x4
                        //dzielisz na 4 obszary, liczysz mean i wariancje, szukasz minimalnej wariancji

                        // Iteracja po masce
                        for (int ky = -padding; ky <= padding; ++ky) {
                            for (int kx = -padding; kx <= padding; ++kx) {
                                int ny = y + ky;
                                int nx = x + kx;
                                    
                                int y_region = ky <= 0 ? 0 : 2;
                                int x_region = kx <= 0 ? 0 : 1;
                                int regionIndex = y_region + x_region;

                                means[regionIndex] += colors[ny][nx][c];
                            }
                        }

                        for (auto& mean : means) { mean = mean / region_size; }
                               
                        // Musimy przejść znowu żeby obliczyć wariancje
                        for (int ky = -padding; ky <= padding; ++ky) {
                            for (int kx = -padding; kx <= padding; ++kx) {
                                int ny = y + ky;
                                int nx = x + kx;

                                int y_region = ky <= 0 ? 0 : 2;
                                int x_region = kx <= 0 ? 0 : 1;
                                int regionIndex = y_region + x_region;

                                variances[regionIndex] += pow((means[regionIndex] - colors[ny][nx][c]), 2);
                            }
                        }

                        for (auto& variance : variances) { variance = variance / region_size; }

                        // Znalezienie regionu o minimalnej wariancji
                        int bestRegion = 0;
                        for (int i = 1; i < 4; ++i) {
                            if (variances[i] < variances[bestRegion]) {
                                bestRegion = i;
                            }
                        }

                        //przypisanie z regionu o min wariancji
                        output[y][x][c] = static_cast<unsigned char>(means[bestRegion]);
                    }
                }
            }
        });
}

int main(int argc, char* argv[]) {


    int grains[] = { 1, 10, 100, 500, 1000 };
    bool timed = true;
    
    int width, height = 0;
    std::string filepath = "building.jpg"; // Ścieżka do pliku wejściowego

  
    //wczytaj
    unsigned char*** colors = loadImageColors(filepath, width, height);
    unsigned char*** output = loadImageColors(filepath, width, height);
    unsigned char*** kuwahara_output = loadImageColors(filepath, width, height);
    if (timed) {
        std::ofstream file;
        file.open("filtry_wyniki.csv", std::ofstream::out | std::ofstream::app);
        file << width << "x" << height << "\n";
        for (auto& grain : grains) {
            file << "Ziarno: " << grain << "\n";
            file << "normal, kuwahara" << "\n";
            std::cout << "Ziarno: " << grain << "\n";
            std::cout << "normal | kuwahara" << std::endl;
            for (int i = 0; i < 5; i++) {
                tbb::tick_count t1 = tbb::tick_count::now();
                applyFilterParallel(colors, output, width, height, mean7x7, grain);
                tbb::tick_count t2 = tbb::tick_count::now();

                file << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << ",";
                tbb::tick_count t3 = tbb::tick_count::now();
                //poprawny region size to 3x3 - 4, 5x5 - 9, 7x7 - 16
                applyFilterParallelKuwahara(colors, kuwahara_output, width, height, mean7x7, grain, 16);
                tbb::tick_count t4 = tbb::tick_count::now();
                file << std::fixed << std::setprecision(3) << (t4 - t3).seconds() << "\n";

                std::cout << std::fixed << std::setprecision(3) << (t2 - t1).seconds() << "    " << (t4 - t3).seconds() << std::endl;
            }
        }
        file.close();
    }

    else {
        applyFilterParallel(colors, output, width, height, gauss4_5x5, 10);
        applyFilterParallelKuwahara(colors, kuwahara_output, width, height, LAPL3, 10, 4);
    }

    std::string outputPath = std::to_string(width) + "x" + std::to_string(height) + "_nieistotne.ppm";
    std::string outputPathKuwahara = std::to_string(width) + "x" + std::to_string(height) + "_nieistotne2.ppm";

    //zapisanie obrazu do formatu PPM
    saveImageToPPM(output, width, height, outputPath);
    saveImageToPPM(kuwahara_output, width, height, outputPathKuwahara);
    //zwolnij pamięć
    freeImageColors(colors, width, height);
    freeImageColors(output, width, height);
    freeImageColors(kuwahara_output, width, height);

    
    return 0;
}
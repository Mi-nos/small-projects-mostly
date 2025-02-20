 /*
 c program:
 --------------------------------
  1. draws Mandelbrot set for Fc(z)=z*z +c
  using Mandelbrot algorithm ( boolean escape time )
 -------------------------------
 2. technique of creating ppm file is  based on the code of Claudio Rocchini
 http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
 create 24 bit color graphic file ,  portable pixmap file = PPM
 see http://en.wikipedia.org/wiki/Portable_pixmap
 to see the file use external application ( graphic viewer)
  */
 #include <stdio.h>
 #include <cstdlib>
 #include <math.h>
 #include <vector>
 #include <thread>
 #include <chrono>
 #include <fstream>
 #include <iostream>
 #include <string>
 #include <mutex>
 #include <algorithm>

static int num_threads = 1;
static int max_threads = 4;
static int num_tests = 1;

const int iXmax = 1600;
const int iYmax = 1600;

std::vector<double> thread_times;
std::vector<int> thread_iters;
std::mutex vector_access;

const double CxMin=-2.5;
const double CxMax=1.5;
const double CyMin=-2.0;
const double CyMax=2.0;

const int MaxColorComponentValue=255;
const int IterationMax=200;
const double EscapeRadius=2;

const double PixelWidth=(CxMax-CxMin)/iXmax;
const double PixelHeight=(CyMax-CyMin)/iYmax;

unsigned char color[iYmax][iXmax][3];
const double ER2=EscapeRadius*EscapeRadius;

void call_from_thread(int tid){
    int fullIterations = 0;
    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2;
    int Iteration;
    int lb = (iYmax/num_threads) * tid;
    int ub = lb + (iYmax/num_threads) - 1;
    int iX, iY;
    auto start{std::chrono::steady_clock::now()};
    for (iY = lb; iY <= ub; iY++) {
        Cy=CyMin + iY*PixelHeight;
        if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
        for(iX=0; iX<iXmax; iX++)
        {
            Cx=CxMin + iX*PixelWidth;
            /* initial value of orbit = critical point Z= 0 */
            Zx=0.0;
            Zy=0.0;
            Zx2=Zx*Zx;
            Zy2=Zy*Zy;
            /* */
            for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
            {
                Zy=2*Zx*Zy + Cy;
                Zx=Zx2-Zy2 +Cx;
                Zx2=Zx*Zx;
                Zy2=Zy*Zy;
            };
            /* compute  pixel color (24 bit = 3 bytes) */
            if (Iteration==IterationMax)
            { /*  interior of Mandelbrot set = black */
                color[iY][iX][0]=0;
                color[iY][iX][1]=0;
                color[iY][iX][2]=0;
                fullIterations++;
            }
            else
            { /* exterior of Mandelbrot set = white */
                color[iY][iX][0]=255 - (tid*10); /* Red*/
                color[iY][iX][1]=255 - (tid*10);  /* Green */
                color[iY][iX][2]=255 - (tid*10);/* Blue */
            };
        }
    }
    auto end{std::chrono::steady_clock::now()};
    std::chrono::duration<double> elapsed_seconds{end - start};
    vector_access.lock();
    thread_times.push_back(elapsed_seconds.count());
    thread_iters.push_back(fullIterations);
    vector_access.unlock();

    //std::cout<<"Thread: " << tid << " ended its work after: " << elapsed_seconds.count() << "s. It made: " << fullIterations << " full iterations. Waiting for other threads..." << "\n";
}
void mandelbrot()
 {
          /* screen ( integer) coordinate */

        int iX,iY;
        /* world ( double) coordinate = parameter plane*/
        double Cx,Cy;
        FILE * fp;
        std::string name = std::to_string(iXmax) + "_sekwencyjnie.ppm";
        const char *filename= name.c_str();
        char *comment="# ";/* comment should start with # */
        static unsigned char color[3];
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
        int Iteration;
        fp= fopen(filename,"wb"); /* b -  binary mode */
        fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
        for(iY=0;iY<iYmax;iY++)
        {
             Cy=CyMin + iY*PixelHeight;
             if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
             for(iX=0;iX<iXmax;iX++)
             {
                        Cx=CxMin + iX*PixelWidth;
                        /* initial value of orbit = critical point Z= 0 */
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;
                        Zy2=Zy*Zy;
                        /* */
                        for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy;
                            Zx=Zx2-Zy2 +Cx;
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
                           color[0]=0;
                           color[1]=0;
                           color[2]=0;
                        }
                     else
                        { /* exterior of Mandelbrot set = white */
                             color[0]=255; /* Red*/
                             color[1]=255;  /* Green */
                             color[2]=255;/* Blue */
                        };
                        /*write color to the file*/
                        fwrite(color,1,3,fp);
                }
        }
        fclose(fp);
 }
 int main(int argc, char* argv[])
 {
 ////////////////////////////////////////////SEKCJA RÓWNOLEGŁA
        if(argv[1]) {max_threads = std::atoi(argv[1]); }
        if(argv[2]) {num_tests = std::atoi(argv[2]); }

        std::vector<std::thread> thread_vector;
        std::ofstream file;
        file.open("results.csv", std::ofstream::out | std::ofstream::app);
        file << iXmax << "\n";
        for (int j = 1; j <= max_threads; j = j*2) {
            num_threads = j;
            FILE * fp;
            std::string name = std::to_string(num_threads) + "_" + std::to_string(iXmax) + "_rownolegle.ppm";
            const char *filename= name.c_str();
            char *comment="# ";
            fp= fopen(filename,"wb");
            fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);



            file << num_threads << "," << "Idle-time" << "," << "Roznica iteracji petli" << "\n";
            for(int k = 0; k < num_tests; k++) {
                auto start{std::chrono::steady_clock::now()};
                for (int i = 0; i < num_threads; i++) { thread_vector.push_back(std::thread(call_from_thread, i));}
                for(auto &t : thread_vector) { t.join(); }
                fwrite(color, sizeof(unsigned char), iYmax * iXmax * 3, fp);
                thread_vector.clear();
                auto end{std::chrono::steady_clock::now()};
                std::chrono::duration<double> elapsed_seconds{end - start};
                //print elementów wektora
                auto minmax_times = std::minmax_element(thread_times.begin(), thread_times.end());
                auto minmax_iters = std::minmax_element(thread_iters.begin(), thread_iters.end());
                std::cout << "Idle-time: " << (*minmax_times.second)-(*minmax_times.first) << std::endl;
                std::cout << "Roznica w iteracjach: " << (*minmax_iters.second)-(*minmax_iters.first) << std::endl;
                //wyczysc wektory czasów i wątkow
                thread_iters.clear();
                thread_times.clear();



                std::cout << elapsed_seconds.count() << "s\n";
                file << elapsed_seconds.count() << "," << (*minmax_times.second)-(*minmax_times.first) << "," << (*minmax_iters.second)-(*minmax_iters.first) <<"\n";
            }
        fclose(fp);
        std::cout <<"\n";
        }
        file << "\n";


        bool seq = true;
 //////////////////////////////////////////////KONIEC SEKCJI RÓWNOLEGŁEJ
 //nie musisz generować ich więcej boooooo mają takie same czasy i nic nie wnoszą
        if(seq) {
            for(int k = 0; k < num_tests; k++) {
                auto start{std::chrono::steady_clock::now()};
                mandelbrot();
                auto end{std::chrono::steady_clock::now()};
                std::chrono::duration<double> elapsed_seconds{end - start};
                std::cout << elapsed_seconds.count() << "s\n";
                file << elapsed_seconds.count() << "\n";
            }
        }


        file.close();
        return 0;
 }
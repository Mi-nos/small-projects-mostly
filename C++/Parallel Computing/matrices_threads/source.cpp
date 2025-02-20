#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
static  int N = 1000;
static  int num_threads = 4;
static  int num_tests = 5;
static  bool tposed = false;
std::vector<std::vector<int>> a(N, std::vector<int>(N, 0));
std::vector<std::vector<int>> b(N, std::vector<int>(N, 0));
std::vector<std::vector<int>> c(N, std::vector<int>(N, 0));
std::vector<std::vector<int>> bt(N, std::vector<int>(N, 0));
void call_from_thread(int tid){
	//tu liczmy rownolegle
	int lb = (N/num_threads) * tid;
	int ub = lb + (N/num_threads) - 1;
	for(int i = lb; i <= ub; i++){
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < N; k++) { c[i][j] += a[i][k] * b[k][j]; }
		}
	}
}
//to samo ale z transpozycja
void call_from_thread_t(int tid) {
	//tu liczmy rownolegle
	int lb = (N/num_threads) * tid;
	int ub = lb + (N/num_threads) - 1;
	for(int i = lb; i <= ub; i++){
		for(int j = 0; j < N; j++) { bt[i][j] = b[j][i]; }
	}
	for(int i = lb; i <= ub; i++){
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < N; k++) { c[i][j] += a[i][k] * bt[j][k]; }
		}
	}
}
int main(int argc, char* argv[]) {
    std::ofstream file;
    file.open("output.csv", std::ofstream::out | std::ofstream::app);
    std::vector<std::thread> thread_vector;
    if (argv[1]) {N = std::atoi(argv[1]); }
    if (argv[2]) {num_threads = std::atoi(argv[2]); }
    if (argv[3]) {num_tests = std::atoi(argv[3]); }
    if (argv[4]) {tposed = std::atoi(argv[4]); }

    file << N << "," << num_threads << "\n";
    //mnozenie rownolegle
    for (int k = 0; k < num_tests; k++) {
        std::cout <<"Rownolegle"<<std::endl;
        auto start{std::chrono::steady_clock::now()};

        for (int i = 0; i < num_threads; i++) {
            if(tposed == true) { thread_vector.push_back(std::thread(call_from_thread_t, i)); }
            else { thread_vector.push_back(std::thread(call_from_thread, i)); }
        }
        for(auto &t : thread_vector) { t.join(); }
        //dla wielu testow musisz wyczyscic wektor watkow
        thread_vector.clear();
        auto end{std::chrono::steady_clock::now()};
        std::chrono::duration<double> elapsed_seconds{end - start};
        std::cout << elapsed_seconds.count() << "s\n";
        file << elapsed_seconds.count() << ",";

	    //mnozenie sekwencyjne
	    std::cout <<"Sekwencyjnie"<<std::endl;
	    start = std::chrono::steady_clock::now();
	    for(int i = 0; i < N; i++) {
		    for(int j = 0; j < N; j++) {
			    for(int k = 0; k < N; k++) { c[i][j] += a[i][k] * b[k][j]; }
		    }
	    }
	    end = std::chrono::steady_clock::now();
	    elapsed_seconds = end - start;
        std::cout << elapsed_seconds.count() << "s\n";
        file << elapsed_seconds.count() << "\n";
   }
   file.close();
   return 0;
}
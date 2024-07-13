#define _USE_MATH_DEFINES
#include <iostream>
#include <complex>
#include <chrono>
#include <math.h>
using namespace std::literals;
const double pi = acos(-1.0);
std::complex<double> ii(0.0, 1.0);
std::complex<double> M_I_2PI = -2.0 * pi * ii; // -(6.2831853071i);
void DFT(double* f, std::complex<double>* output, int N) {
	
	for (int k = 0; k < N; k++) {
		output[k] = 0.0;
		for (int n = 0; n < N; n++) {
			output[k] += f[n] * std::exp(M_I_2PI * (double)k * (double)n  / (double)N);
			
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_REC(std::complex<double>* f, int N);

void FFT(double* f, std::complex<double>* output, int N) {

	for (int i = 0; i < N; i++) {
		output[i] = std::complex<double>(f[i], 0);
	}
	FFT_REC(output, N);
}

void FFT_REC(std::complex<double>* output, int N) {

	if (N <= 1) {
		return;
	}

	std::complex<double>* even = new std::complex<double>[N / 2];
	std::complex<double>* odd = new std::complex<double>[N / 2];

	for (int i = 0; i < N / 2; i++) {
		even[i] = output[i * 2];
		odd[i] = output[i * 2 + 1];
	}

	FFT_REC(even, N / 2);
	FFT_REC(odd, N / 2);

	for (int k = 0; k < N / 2; k++) {
		std::complex<double> t = exp(std::complex<double>(0, -2 * M_PI * k / N)) * odd[k];
		output[k] = even[k] + t;
		output[N / 2 + k] = even[k] - t;
	}

	delete[] even;
	delete[] odd;
}


int main() {
	const int MAX_ORDER = 13;
	const bool PRINT_COEFS = 0;
	for (int o = 1; o <= MAX_ORDER; o++) {
		const int N = 1 << o;
		printf("N: %i\n", N);
		double* f = new double[N];
		std::complex<double>* outputDFT = new std::complex<double>[N];
		std::complex<double>* outputFFT = new std::complex<double>[N];
		for (int n = 0; n < N; n++) { 
			f[n] = std::pow(n/(double)N,2); 
			//std::cout << f[n] << std::endl;
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		//tutaj transformata dft

		DFT(f,outputDFT, N);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "DFT time taken: " << timeTaken.count() << std::endl;

		t1 = std::chrono::high_resolution_clock::now();
		//tutaj transformata fft
		FFT(f, outputFFT, N);
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
		std::cout << "FFT time taken: " << timeTaken.count() << std::endl;
		//tutaj blad sredni
		double meanError = 0;
		for (int k = 0; k < N; k++) {
			meanError = meanError + (abs(outputDFT[k] - outputFFT[k]));
		}
		meanError = meanError / double(N);

		std::printf("error: %le\n", meanError);
		//std::cout << "Sredni modul roznic: " << meanError << std::endl;
		if (PRINT_COEFS) { 
			std::cout << "--------------------------------------" << std::endl;
			std::cout << "Wyniki DFT: " << std::endl;
			for (int k = 0; k < N; k++) { std::cout << "R: " << outputDFT[k].real() << " I: " << outputDFT[k].imag() << std::endl; }
			std::cout << "--------------------------------------" << std::endl;
		}

		if (PRINT_COEFS) {
			std::cout << "Wyniki FFT: " << std::endl;
			for (int k = 0; k < N; k++) { std::cout << "R: " << outputFFT[k].real() << " I: " << outputFFT[k].imag() << std::endl; }
			std::cout << "--------------------------------------" << std::endl;
		}

		delete [] f;
		delete[] outputDFT;
		delete[] outputFFT;
	}



	return 0;
}



//void FFTrecTEMP(double* f, std::complex<double>* output, int N) {
//	if (N <= 1) { return; }
//
//
//	double* f_even = new double[N / 2];
//	double* f_odd = new double[N / 2];
//
//
//	for (int k = 0; k < N / 2; k++) {
//		f_even[k] = f[2 * k];
//		f_odd[k] = f[2 * k + 1];
//	}
//
//	FFTrecTEMP(f_even, output, N / 2);
//	FFTrecTEMP(f_odd, output, N / 2);
//	for (int k = 0; k < N / 2; k++) {
//		std::complex<double> temp = std::exp(M_I_2PI * (double)k / (double)N) * f_odd[k];
//		output[k] = f_even[k] + temp;
//		output[k + (N / 2)] = f_even[k] - temp;
//	}
//
//	delete[] f_even;
//	delete[] f_odd;
//
//}
//
//std::complex<double>* FFTTEMP(double* f, std::complex<double>* output, int N) {
//	for (int k = 0; k < N; k++) { output[k] = 0.0; }
//	FFTrecTEMP(f, output, N);
//	return output;
//}
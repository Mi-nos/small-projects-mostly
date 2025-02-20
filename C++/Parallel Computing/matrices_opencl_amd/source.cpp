#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <CL/cl.h>

void printMaxWorkItemSizes(cl_device_id device) {
	size_t maxWorkItemSizes[3] = { 0, 0, 0 };
	cl_int err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
		sizeof(maxWorkItemSizes), maxWorkItemSizes, nullptr);
	if (err != CL_SUCCESS) {
		std::cerr << "Error getting CL_DEVICE_MAX_WORK_ITEM_SIZES: " << err << std::endl;
	}
	else {
		std::cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES:" << std::endl;
		std::cout << "  Dimension 0: " << maxWorkItemSizes[0] << std::endl;
		std::cout << "  Dimension 1: " << maxWorkItemSizes[1] << std::endl;
		std::cout << "  Dimension 2: " << maxWorkItemSizes[2] << std::endl;
	}
}

void printKernelWorkGroupSize(cl_kernel kernel, cl_device_id device) {
	size_t workGroupSize = 0;
	cl_int err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
		sizeof(size_t), &workGroupSize, nullptr);
	if (err != CL_SUCCESS) {
		std::cerr << "Error getting CL_KERNEL_WORK_GROUP_SIZE: " << err << std::endl;
	}
	else {
		std::cout << "CL_KERNEL_WORK_GROUP_SIZE: " << workGroupSize << std::endl;
	}

}

void getDeviceInfo() {
	cl_int err;
	cl_uint platformCount;
	std::vector<cl_platform_id> platforms;

	err = clGetPlatformIDs(0, nullptr, &platformCount);
	if (err != CL_SUCCESS) {
		std::cerr << "Failed to get OpenCL platforms!" << std::endl;
		return;
	}
	platforms.resize(platformCount);
	clGetPlatformIDs(platformCount, platforms.data(), nullptr);
	std::cout << "Found " << platformCount << " OpenCL platforms." << std::endl;
	for (cl_uint i = 0; i < platformCount; ++i) {
		char buffer[1024];
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, buffer, nullptr);
		std::cout << "Platform " << i + 1 << ": " << buffer << std::endl;
	}
	cl_platform_id selectedPlatform = platforms[0]; // Wybierz AMD (pierwsza platforma w tablicy)
	cl_uint deviceCount;
	err = clGetDeviceIDs(selectedPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);
	if (err != CL_SUCCESS || deviceCount == 0) {
		std::cerr << "Failed to get devices for selected platform!" << std::endl;
		return;
	}
	std::vector<cl_device_id> devices(deviceCount);
	err = clGetDeviceIDs(selectedPlatform, CL_DEVICE_TYPE_ALL, deviceCount, devices.data(), nullptr);
	if (err != CL_SUCCESS) {
		std::cerr << "Failed to get device IDs!" << std::endl;
		return;
	}
	// Wyświetlenie informacji o urządzeniach
	for (cl_uint i = 0; i < deviceCount; ++i) {
		char deviceName[1024];
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
		std::cout << "Selected Platform Device " << i + 1 << ": " << deviceName << std::endl;
	}
	for (cl_uint i = 0; i < deviceCount; ++i) {
		char deviceName[1024];
		cl_ulong globalMem;
		cl_uint computeUnits;
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMem), &globalMem, nullptr);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);
		std::cout << "Device " << i + 1 << ": " << deviceName << std::endl;
		std::cout << " Global Memory: " << globalMem / (1024 * 1024) << " MB" << std::endl;
		std::cout << " Compute Units: " << computeUnits << std::endl;
	}
}










int main2() {
	getDeviceInfo();
	return 0;
}


int main() {
	// Inicjalizacja danych macierzy
	const unsigned int matrix_sizes[] = { 2048, 4096, 8192, 16384 }; //
	const unsigned int work_group_sizes[] = { 4, 8, 16 };
	std::ofstream file;
	file.open("openCL_wyniki.csv", std::ofstream::out | std::ofstream::app);

	// Inicjalizacja OpenCL
	cl_int err;
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, nullptr);
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
	cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
	const cl_queue_properties properties[] = {
        CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE,
        0 // Zakończenie listy właściwości
    };
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, properties, &err);

	// Załadowanie kodu kernelu
	const char* kernelSource = R"(
	__kernel void matrix_multiply_2d(
		__global const float* A, // macierz A
		__global const float* B, // macierz B
		__global float* C, // macierz wynikowa C
		const unsigned int M, // liczba wierszy w macierzy A i C
		const unsigned int N, // liczba kolumn w macierzy A
		const unsigned int K,
		__local float* As,
		__local float* Bs,
		const unsigned int H) // liczba kolumn w macierzy B i wierszy w macierzy A
	{
		// Indeks wątku w 2D
		unsigned int row = get_global_id(1); // Wiersz w macierzy C
		unsigned int col = get_global_id(0); // Kolumna w macierzy C
		// Pamięć lokalna
		int localIndex = get_local_id(1) * get_local_size(0) + get_local_id(0);
		As[localIndex] = 0;
		Bs[localIndex] = 0;
		barrier(CLK_LOCAL_MEM_FENCE);
		float result = 0.0f;
		// Mnożenie macierzy C = A * B, podzielone na fragmenty
		for (unsigned int i = 0; i < (K / H); ++i) {
			// Załaduj fragmenty A i B do pamięci lokalnej
			As[localIndex] = A[row * K + (i * H + get_local_id(0))];
			Bs[localIndex] = B[(i * H + get_local_id(1)) * N + col];
			// Czekaj na załadowanie danych
			barrier(CLK_LOCAL_MEM_FENCE);
			// Wykonaj mnożenie
			for (unsigned int j = 0; j < H; ++j) {
				result += As[get_local_id(1) * H + j] * Bs[j * H + get_local_id(0)];
			}
			// Czekaj na zakończenie obliczeń przed załadowaniem nowych fragmentów
			barrier(CLK_LOCAL_MEM_FENCE);
		}
		C[row * N + col] = result;
	}
	)";

	cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
	clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

	if (err != CL_SUCCESS) {
		// Obsługa błędu
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		std::vector<char> buildLog(logSize);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, buildLog.data(), nullptr);
		std::cerr << "Build Error:" << std::endl;
		std::cerr << buildLog.data() << std::endl;
	}

	// Alokacja pamięci na dane
	for (auto& m_size : matrix_sizes) {
		std::cout << "Matrix size: " << m_size << "X" << m_size << std::endl;
		file << m_size << "X" << m_size << "\n";
		for (auto& w_size : work_group_sizes) {
			file << "Work group size: " << w_size << "X" << w_size << "\n";
			std::cout << "Rozmiar grupy roboczej: " << w_size << "X" << w_size << std::endl;
			for (int i = 0; i < 5; i++) {
				const unsigned int M = m_size, N = m_size, K = m_size, H = w_size;

				std::vector<float> A(M * K, 1.0f); // Macierz A
				std::vector<float> B(K * N, 1.0f); // Macierz B
				std::vector<float> C(M * N, 0.0f); // Macierz wynikowa C

				cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * M * K, A.data(), &err);
				cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * K * N, B.data(), &err);
				cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * M * N, nullptr, &err);

				size_t localMemSize = H * H * sizeof(float); // Obliczenie rozmiaru
				cl_kernel kernel = clCreateKernel(program, "matrix_multiply_2d", &err);
				//printMaxWorkItemSizes(device);
				//printKernelWorkGroupSize(kernel, device);
				// Przekazanie danych do kernela
				clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
				clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
				clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
				clSetKernelArg(kernel, 3, sizeof(unsigned int), &M);
				clSetKernelArg(kernel, 4, sizeof(unsigned int), &N);
				clSetKernelArg(kernel, 5, sizeof(unsigned int), &K);
				clSetKernelArg(kernel, 6, localMemSize, nullptr);
				clSetKernelArg(kernel, 7, localMemSize, nullptr);
				clSetKernelArg(kernel, 8, sizeof(unsigned int), &H);

				// Ustawienie rozmiaru grupy roboczej i globalnego rozmiaru
				size_t globalSize[] = { M, N };
				size_t localSize[] = { H, H }; // Wymiary grupy roboczej
				// Uruchomienie kernela
				cl_event event;
				err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, localSize, 0, nullptr, &event);
				clWaitForEvents(1, &event);
				clFinish(queue);

				cl_ulong time_start, time_end;
				clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, nullptr);
				clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, nullptr);
				double nanoSeconds = time_end - time_start;
				// Pobranie wyników

				err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * M * N, C.data(), 0, nullptr, nullptr);
				// Wyświetlenie wyników
				std::cout << "C[0] = " << C[0]  << ". Czas: " << nanoSeconds / 1e9 << std::endl;
				file << std::fixed << std::setprecision(3) << nanoSeconds / 1e9 << "\n";
				// Czyszczenie zasobów
				clReleaseEvent(event);
				clReleaseMemObject(bufferA);
				clReleaseMemObject(bufferB);
				clReleaseMemObject(bufferC);
				clReleaseKernel(kernel);
			}
		}
	}
	file.close();
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	return 0;

}
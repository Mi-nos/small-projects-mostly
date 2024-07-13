#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <list>
#include <random>
struct Pudlo {
	double key = 0;
	char dane = 'a';

	Pudlo() {
		key = -1;
		dane = 'z';
	}
	Pudlo(double key, char dane) {
		this->key = key;
		this->dane = dane;
	}

	//Pudlo& rozpakuj(Pudlo* pudelko) { return *pudelko; }
	friend std::ostream& operator<<(std::ostream& os, const Pudlo*& p1) {
		if (p1) { return (os << p1->key << " + " << p1->dane); }
		else { return (os << "nullptr"); }
	} //to siê nigdy nie stanie
	friend std::ostream& operator<<(std::ostream& os, const Pudlo& p1) { return (os << p1.key << " ++ " << p1.dane); }

};

template <class T>
class Heap {
private:

	int size = 0;
	int capacity = 1;
	int multiplier = 4;
	T* container;

	//IT JUST WORKS
	template <typename U>
	static std::string printElement(const U& element) {
		std::ostringstream out;
		if constexpr (std::is_pointer_v<U>) {
			// Sprawdź, czy U to wskaźnik
			if (element) {
				out << *element;
			}
			else {
				out << "nullptr";
			}
		}
		else {
			out << element; // Dla innych typów, normalnie użyj operatora <<
		}
		return out.str();
	}

public:

	Heap() { container = new T[capacity]; }
	Heap(T* container, int size, double (*cmp)(T t1, T t2), bool variant) {
		this->container = container;
		this->size = size;
		capacity = size; //zakladamy ze tablica jest zapelniona
		fixHeap(variant, cmp);
	}
	~Heap() {
		delete[] container;
	}
	void addElement(T data) {
		if (size == capacity) {
			T* newContainer = new T[capacity * multiplier];
			for (int i = 0; i < size; i++) { newContainer[i] = container[i]; }

			newContainer[size] = data;
			size++;
			capacity = capacity * multiplier;
			delete[] container;
			container = newContainer;
		}
		else {
			container[size] = data;
			size++;
		}
	}
	void deleteData() {
		T* newContainer = new T[1];
		delete[] container;
		container = newContainer;
		size = 0;
		capacity = 1;
	}

	std::string toString(int howMany) { //string dla obiektow
		std::ostringstream out;
		out << "Kopiec. \nObecny rozmiar: " << size << "\nObecna pojemnosc: " << capacity << std::endl;
		std::string output = out.str();
		out.str("");
		if (howMany > size || howMany < 0) { throw std::out_of_range("Niepoprawny indeks!"); }
		for (int i = 0; i < howMany; i++) {
			if (std::is_pointer<T>::value) { out << "Indeks: " << i << " Dane: " << printElement(container[i]) << std::endl; }
			else { out << "Indeks: " << i << " Dane: " << container[i] << std::endl; }
		}
		output = output + out.str();
		return output;
	}


	int parent(int i) { return std::floor((i - 1) / 2); }
	int left(int i) { return 2 * i + 1; }
	int right(int i) { return 2 * i + 2; }

	void swapNodes(T& A, T& B) {
		T temp = A;
		A = B;
		B = temp;
	}

	void heapUp(int i, double (*cmp)(T t1, T t2)) {
		while (i > 0 && cmp(container[parent(i)], container[i]) <= 0) {
			swapNodes(container[parent(i)], container[i]);
			i = parent(i);
		}
	}

	void heapDown(int i, double (*cmp)(T t1, T t2)) {
		while (i < size) {
			int winnerIndex = i;
			if (left(i) < size && cmp(container[winnerIndex], container[left(i)]) <= 0) { winnerIndex = left(i); }
			if (right(i) < size && cmp(container[winnerIndex], container[right(i)]) <= 0) { winnerIndex = right(i); }

			if (winnerIndex != i) {
				swapNodes(container[i], container[winnerIndex]);
				i = winnerIndex;
			}
			else { break; }

		}

	}
	void addtoHeap(T data, double (*cmp)(T t1, T t2)) { //A
		addElement(data); //ETAP 1: umiesc element na samym koncu tablicy
		int i = size - 1;
		heapUp(i, cmp);		//ETAP 2: wykopcuj sie tak wysoko jak mozesz
	}
	T pollRoot(int (*cmp)(T t1, T t2)) {
		if (size == 0) { return T(); }

		T giveBack = container[0];
		swapNodes(container[0], container[size - 1]);
		size--;
		heapDown(0, cmp);

		return giveBack;

	}
	void fixHeap(bool variant, double (*cmp)(T t1, T t2)) {
		if (variant == 0) {
			for (int i = size / 2 - 1; i >= 0; i--) { //budujemy od dolu w gore bottom-up
				heapDown(i, cmp);
			}
		}
		else {
			int i = 0;
			while (i < size) {	// topdown
				heapUp(i, cmp);
				i++;
			}
		}

	}


	void sort(double (*cmp)(T t1, T t2)) {
		int rozmiar = size;
		while (size > 0) {
			swapNodes(container[0], container[size - 1]); //zamien roota z ostatnim elementem, ostatni element jest teraz na dobrej pozycji
			size--;
			heapDown(0, cmp);

		}
		size = rozmiar;
	}

};


template <typename T>
int comp(T t1, T t2) { return t1 - t2; }
template<>
int comp(Pudlo p1, Pudlo p2) { /*std::cout << "Pudla" << std::endl;*/ return p1.key - p2.key; }
template<typename T>
int comp(T* t1, T* t2) { /*std::cout << "wskazniki" << std::endl;*/ return *(t1)-*(t2); } //ostatnie zmiany
template<>
int comp(Pudlo* p1, Pudlo* p2) { /*std::cout << "Pudla-wskazniki" << std::endl;*/ return p1->key - p2->key; }

template <typename T>
double comp1(T t1, T t2) { return t1 - t2; }
template<>
double comp1(Pudlo p1, Pudlo p2) { /*std::cout << "Pudla" << std::endl;*/ return p1.key - p2.key; }
template<typename T>
double comp1(T* t1, T* t2) { /*std::cout << "wskazniki" << std::endl;*/ return *(t1)-*(t2); } //ostatnie zmiany
template<>
double comp1(Pudlo* p1, Pudlo* p2) { /*std::cout << "Pudla-wskazniki" << std::endl;*/ return p1->key - p2->key; }

void counting_sort(int* arr, int arrSize, int m) {

	int* temp = new int[m] {0};
	for (int i = 0; i < m; i++) { temp[i] = 0; }
	for (int i = 0; i < arrSize; i++) { temp[arr[i]]++; }


	int index = 0;
	for (int i = 0; i < m; i++) {
		while (temp[i] > 0) {
			arr[index] = i;
			temp[i]--;
			index++;

		}
	}
	delete[] temp;
}

void bucket_sort(int* arr, int n, double m) { //sortowane liczby ze zbioru {0 , ... , m - 1}
	std::list<int>* lista = new std::list<int>[n]; //list tyle ile kubelkow, kubelkow tyle ile danych w tablicy
	double range = m / n; //nasze "omega"


	for (int i = 0; i < n; i++) {
		int bucketNumber = std::floor(arr[i] / range);
		lista[bucketNumber].push_back(arr[i]);
	}
	for (int i = 0; i < n; i ++) { lista[i].sort(); }

	int j = 0;
	for (int i = 0; i < n; i++) {
		for (const auto& element : lista[i]) {
			arr[j] = element;
			j++;
		}
	}
	delete [] lista;
}
template<class T>
void bucket_sort(T* arr, int n, double m, double (*cmp)(T t1, T t2) ) {
	std::list<T>* lista = new std::list<T>[n];
	double range = m / n; //nasze "omega"
	for (int i = 0; i <n; i++) {
		int bucketNumber = std::floor(arr[i]->key / range);
		lista[bucketNumber].push_back(arr[i]);
	}
	for (int i = 0; i < n; i++) { lista[i].sort(cmp); }

	int j = 0;
	for (int i = 0; i < n; i++) {
		for (const auto& element : lista[i]) {
			arr[j] = element;
			j++;
		}
	}
	delete[] lista;
}

bool sameArrays(int* tablica1, int* tablica2, int size) {
	for (int i = 0; i < size; i++) {
		if (tablica1[i] != tablica2[i]) {
			std::cout << "ROZNE" << std::endl;
			return false;  // Tablice różnią się przynajmniej w jednym elemencie
		}
	}
	std::cout << "IDENTYCZNE" << std::endl;
	return true;  // Tablice są identyczne
}

bool sameArrays(int* tablica1, int* tablica2, int* tablica3, int rozmiar) {
	for (int i = 0; i < rozmiar; i++) {
		if (tablica1[i] != tablica2[i] || tablica1[i] != tablica3[i] || tablica2[i] != tablica3[i]) {
			std::cout << "ROZNE" << std::endl;
			return false;  // Tablice różnią się przynajmniej w jednym elemencie
		}
	}
	std::cout << "IDENTYCZNE" << std::endl;
	return true;  // Tablice są identyczne
}
template<class T>
bool sameArrays(T* tablica1, T* tablica2, int size) {
	for (int i = 0; i < size; i++) {
		if (tablica1[i]->key != tablica2[i]->key) {
			std::cout << "ROZNE" << std::endl;
			return false;  // Tablice różnią się przynajmniej w jednym elemencie
		}
	}
	std::cout << "IDENTYCZNE" << std::endl;
	return true;  // Tablice są identyczne
}





int main12313() {
	const int MAX_ORDER = 7;
	Heap<Pudlo*>* bh = new Heap<Pudlo*>;

	const int n = (int) pow(10, 7);
	int* array1 = new int[n];
	for (int i = 0; i < n; i++) {
		int val = (rand() << 15) + rand();
		array1[i] = val;
	}
	bh->sort(comp1);
	std::cout << bh->toString(10) << std::endl;
	delete bh;
	return 0;
}


int main()
{
	srand(time(NULL));
	const int MAX_ORDER = 6; // maksymalny rzad wielkosci sortowanych danych
	const int m = (int)pow(10, 7); // sortowane liczby ze zbioru {0 , ... , m - 1}
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = (int)pow(10, o); // rozmiar tablicy z danymi
		int* array1 = new int[n];
		for (int i = 0; i < n; i++)
		{
			int rand_val = ((rand() << 15) + rand()) % m; // tu losowanie liczby calkowitej
				array1[i] = rand_val;
		}

		// skrotowy wypis tablicy do posortowania ( np . pewna liczba poczatkowych elementow )
		std::cout << "Tablica przed posortowaniem: " << std::endl;
		std::cout << "\n";
		for (int i = 0; i < 10; i++) { std::cout << array1[i] << std::endl; }
		std::cout << "\n";
		int* array2 = new int[n];
		int* array3 = new int[n];
		std::memcpy(array2, array1, n * sizeof(int)); // pierwsza kopia
		std::memcpy(array3, array1, n * sizeof(int)); // druga kopia


		//sortowanie przez zliczanie ( do wykonania w miejscu )
		auto t1 = std::chrono::high_resolution_clock::now();
		counting_sort(array1, n, m);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji sortowania przez zliczanie w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Pierwsze 10 elementow" << std::endl;
		for (int i = 0; i < 10; i++) { std::cout << array1[i] << std::endl; } //pierwsze 10 elementow
		std::cout << "\n" << std::endl;
		std::cout << "Ostatnie 10 elementow" << std::endl;
		for (int i = n - 10; i < n; i++) { std::cout << array1[i] << std::endl; } //ostatnie 10 elementow;
		std::cout << "\n" << std::endl;


		//// sortowanie przez kopcowanie ( do wykonania w miejscu )
		t1 = std::chrono::high_resolution_clock::now();
		Heap < int >* bh = new Heap < int >(array2, n, comp1, 0); // konstruktor kopca z	mozliwoscia przekazania zewnetrznej tablicy(ostatni argument wskazuje kierunek naprawy :
		//// top - down lub bottom - up) kopiec jest zorientowany na maksimum
		bh->sort(comp1);
		t2 = std::chrono::high_resolution_clock::now();
		// wypis pomiaru czasu i skrotowej postaci wynikowej tablicy
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji sortowania przez kopcowanie w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << bh->toString(10) << std::endl;
		// sortowanie kubelkowe ( do wykonania w miejscu )
		t1 = std::chrono::high_resolution_clock::now();
		bucket_sort(array3, n, m); // szczegolna wersja bucket sort tylko dla liczb calkowitych
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		////... // wypis pomiaru czasu i skrotowej postaci wynikowej tablicy
		std::cout << "Czas trwania operacji sortowania przez kubelkowanie w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Pierwsze 10 elementow" << std::endl;
		for (int i = 0; i < 10; i++) { std::cout << array3[i] << std::endl; } //pierwsze 10 elementow
		std::cout << "\n" << std::endl;
		std::cout << "Ostatnie 10 elementow" << std::endl;
		for (int i = n - 10; i < n; i++) { std::cout << array3[i] << std::endl; } //ostatnie 10 elementow;
		std::cout << "\n" << std::endl;
		sameArrays(array1, array3, n);
		sameArrays(array1, array2, array3, n);
		////	... // sprawdzenie zgodnosci tablic array1 , array2 , array3 i wypis informacji o zgodnosci na
		////	ekran
		delete[] array1;
		delete[] array2;
		delete[] array3;
	}
	return 0;
}


int main8585()
{
	std::random_device losuj;
	std::mt19937 gen(losuj());

	std::uniform_real_distribution<double> przedzial(0.0, 1.0);
	const int MAX_ORDER = 7; // maksymalny rzad wielkosci sortowanych danych
	const double m_double = (double)pow(2, 30); // mianownik przy ustalaniu losowej liczby//	z m i e n n o p r z e c i n k o w e j
		for (int o = 1; o <= MAX_ORDER; o++)
		{
			const int n = (int)pow(10, o); // rozmiar tablicy z danymi
			Pudlo** array1 = new Pudlo * [n];
			for (int i = 0; i < n; i++)
			{
				Pudlo* so = new Pudlo();
				so->key = przedzial(gen); // przykladowy sposob wylosowania pola//typu double(ktore bedzie stanowilo klucz sortowania)
				so->dane = 'a' + rand() % 26; // przykladowy sposob wylosowania pola typu char
				array1[i] = so;
			}

//			... // skrotowy wypis tablicy do posortowania ( np . pewna liczba poczatkowych elementow )
			for (int i = 0; i < 10; i++) { std::cout << array1[i] << std::endl; }
			Pudlo** array2 = new Pudlo * [n];
			memcpy(array2, array1, n * sizeof(Pudlo*)); // kopia

//			// sortowanie przez kopcowanie
			auto t1 = std::chrono::high_resolution_clock::now();
			Heap < Pudlo* >* bh = new Heap < Pudlo* >(array1, n, comp1, 0); // konstruktor kopca z mozliwoscia przekazania zewnetrznej tablicy ( ostatni argument wskazuje kierunek naprawy : bottom - up lub top - down  )
			bh->sort(comp1);
		    auto t2 = std::chrono::high_resolution_clock::now();
			auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
//			... // wypis pomiaru czasu i skrotowej postaci wynikowej tablicy
			std::cout << "Czas trwania operacji sortowania przez kopcowanie w mikrosekundach: " << timeTaken.count() << std::endl;
			std::cout << bh->toString(10) << std::endl;

//			// sortowanie kubelkowe
			t1 = std::chrono::high_resolution_clock::now();
			bucket_sort < Pudlo* >(array2, n, 1.0, comp1); // trzeci
//			argument wskazuje, ze liczby sa z przedzialu[0, 1]
			t2 = std::chrono::high_resolution_clock::now();
//			... // wypis pomiaru czasu i skrotowej postaci wynikowej tablicy
			std::cout << "Czas trwania operacji sortowania przez kubelkowanie w mikrosekundach: " << timeTaken.count() << std::endl;
			std::cout << "Pierwsze 10 elementow" << std::endl;
			for (int i = 0; i < 10; i++) { std::cout << array2[i] << std::endl; } //pierwsze 10 elementow
			std::cout << "\n" << std::endl;
			std::cout << "Ostatnie 10 elementow" << std::endl;
			for (int i = n - 10; i < n; i++) { std::cout << array2[i] << std::endl; } //ostatnie 10 elementow;
			std::cout << "\n" << std::endl;
//			... // sprawdzenie zgodnosci tablic array1 , array2 i wypis informacji o zgodnosci na ekran
			delete[] array1;
			delete[] array2;
		}
	return 0;
}

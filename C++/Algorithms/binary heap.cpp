#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

struct Pudlo {
	int key = 0;
	char dane = 'a';

	Pudlo() {
		key = -1;
		dane = 'z';
}
	Pudlo(int key, char dane) {
		this->key = key;
		this->dane = dane;
	}

	//Pudlo& rozpakuj(Pudlo* pudelko) { return *pudelko; }
	friend std::ostream& operator<<(std::ostream& os, const Pudlo*& p1) { return (os << p1->key << " + " << p1->dane); } //to siê nigdy nie stanie
	friend std::ostream& operator<<(std::ostream& os, const Pudlo& p1) { return (os << p1.key << " ++ " << p1.dane); }

};

//template<typename T>
//T& rozpakuj(T* t1) { return *t1; }


//struct Node {
//
//	int id;
//	static int UID;
//	T data{};
//
//	Node() { id = -1; };
//	Node(T someData) {
//		data = someData;
//		id = UID++;
//	}
//	~Node() {}
//};


template <class T>
class Heap {
private:

	int size = 0;
	int capacity = 1;
	int multiplier = 4;

	T* container;
public:

	Heap() { container = new T[capacity]; }
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


	std::string toString(int howMany) {
		std::ostringstream out;
		out << "Kopiec. \nObecny rozmiar: " << size << "\nObecna pojemnosc: " << capacity << std::endl;
		std::string output = out.str();
		out.str("");
		if (howMany > size || howMany < 0) { throw std::out_of_range("Niepoprawny indeks!"); }
		bool a = 1;
		for (int i = 0; i < howMany; i++) {
			out << "Indeks: " << i << " Dane: " << container[i] << std::endl; 
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

	void heapUp(int i, int (*cmp)(T t1, T t2)) {
		while (i > 0 && cmp(container[parent(i)], container[i]) <= 0) {
			swapNodes(container[parent(i)], container[i]);
			i = parent(i);
		}
	}

	void heapDown(int i, int (*cmp)(T t1, T t2)) {
		while (i < size) {	//poki nie przekroczylismy dozwolonych indeksow i mamy mozliwosc zejscia w dol, tzn. korzen mniejszy od tych pod nim
			int winnerIndex = i;
			if (left(i) < size && cmp(container[winnerIndex], container[left(i)]) <= 0) { winnerIndex = left(i); }
			if (right(i) < size && cmp(container[winnerIndex], container[right(i)]) <= 0) { winnerIndex = right(i); }
			
			if (winnerIndex != i) {
				swapNodes(container[i], container[winnerIndex]); 
				i = winnerIndex;
			}
			else { break; } //nie musi chyba byc






			//T biggest;
			//T second_biggest;
			//int biggestIndex = 0;
			//int second_biggestIndex = 0;
			//if (cmp(container[left(i)], container[right(i)]) <= 0) { //wybieramy najwieksza wartosc z dwojki dzieci wraz z indeksami
			//	biggest = container[right(i)];
			//	biggestIndex = right(i);
			//	second_biggest = container[left(i)];
			//	second_biggestIndex = left(i);
			//}
			//else {
			//	biggest = container[left(i)];
			//	biggestIndex = left(i);
			//	second_biggest = container[right(i)];
			//	second_biggestIndex = right(i);
			//}

			//if (cmp(container[i], biggest) <= 0) {
			//	swapNodes(container[i], biggest);
			//	i = biggestIndex;

			//}
			//else if (cmp(container[i], second_biggest) <= 0){
			//	swapNodes(container[i], second_biggest);
			//	i = second_biggestIndex;
			//}


		}

	}
	void addtoHeap(T data, int (*cmp)(T t1, T t2)) { //A
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

};


template <typename T>
int comp(T t1, T t2) { return t1 - t2; }
template<>
int comp(Pudlo p1, Pudlo p2) { /*std::cout << "Pudla" << std::endl;*/ return p1.key - p2.key; }
template<typename T>
int comp(T* t1, T* t2) { /*std::cout << "wskazniki" << std::endl;*/ return *(t1)-*(t2); } //ostatnie zmiany
template<>
int comp(Pudlo* p1, Pudlo* p2) { /*std::cout << "Pudla-wskazniki" << std::endl;*/ return p1->key - p2->key; }


//wskazniki na pudla
int mainPUDWSKA() {
	const int MAX_ORDER = 7;
	Heap<Pudlo*>* bh = new Heap < Pudlo* >();
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		auto t1 = std::chrono::high_resolution_clock::now();
		//sekcja na dodawanie danych
		for (int i = 0; i < n; i++)
		{	

			int val = (rand() << 15) + rand();
			Pudlo* temp = new Pudlo(val, 'a');
			bh->addtoHeap(temp, comp);

		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << bh->toString(10) << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl;

		//sekcja na pollowanie
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			auto it = bh->pollRoot(comp);
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji pollowania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji pollowania w mikrosekundach: " << timeTaken.count() / n << std::endl;
		bh->deleteData(); // reset tablicy dynamicznej

	}
	delete bh;






	return 0;
}

//pudla
int main() {
	const int MAX_ORDER = 1;
	Heap<Pudlo>* bh = new Heap < Pudlo >();
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		auto t1 = std::chrono::high_resolution_clock::now();
		//sekcja na dodawanie danych
		for (int i = 0; i < n; i++)
		{
			int val = ((rand() << 15) + rand())%10;
			Pudlo temp(val, 'b');
			bh->addtoHeap(temp, comp);
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << bh->toString(10) << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl;
		//sekcja na pollowanie
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			auto it = bh->pollRoot(comp);
			std::cout << it << std::endl;
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji pollowania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji pollowania w mikrosekundach: " << timeTaken.count() / n << std::endl;
		bh->deleteData(); // reset tablicy dynamicznej

	}
	delete bh;






	return 0;
}






//inty
int mainINT() {
	const int MAX_ORDER = 7;
	Heap<int>* bh = new Heap < int >();
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		auto t1 = std::chrono::high_resolution_clock::now();
		//sekcja na dodawanie danych
		for (int i = 0; i < n; i++)
		{
			int val = (rand() << 15) + rand();
			bh->addtoHeap(val, comp);
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << bh->toString(9) << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl; 
		//sekcja na pollowanie
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			auto it = bh->pollRoot(comp);
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji pollowania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji pollowania w mikrosekundach: " << timeTaken.count() / n << std::endl;
		bh->deleteData(); // reset tablicy dynamicznej

	}
	delete bh;






	return 0;
}
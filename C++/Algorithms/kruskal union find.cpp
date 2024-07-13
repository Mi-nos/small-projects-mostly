#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

//pamięć większa o x 2.0
//1. Zainicjalizuj strukturę Union - Find(na początku każdy węzeł stanowi odrębny jednoelementowy
//	zbiór).
//	2. Posortuj krawędzie grafu według wag rosnąco.
//	3. Dla każdej krawędzi(i, j) w posortowanej kolekcji :
//3.1 Znajdź zbiór, do którego należy węzeł i(operacja find).
//3.2 Znajdź zbiór, do którego należy węzeł j(operacja find).
//3.3 Jeżeli znalezione zbiory dla i oraz j są różne to : dodaj krawędź(i, j) do wynikowego drzewa
//rozpinającego oraz połącz te zbiory(operacja union).
//4. Zwróć wynikowe drzewo rozpinające.

struct Edge {
	int a;
	int b;
	double cost;

	Edge() {
		a = 0;
		b = 1;
		cost = 0;
	}

	Edge(int a, int b, double cost) {
		this->a = a;
		this->b = b;
		this->cost = cost;
	}
	friend std::ostream& operator<<(std::ostream& os, const Edge*& p1) {
		if (p1) { return (os << p1->a << " + " << p1->b << " + " << p1->cost); }
		else { return (os << "nullptr"); }
	} //to siê nigdy nie stanie
	friend std::ostream& operator<<(std::ostream& os, const Edge& p1) { return (os << p1.a << " ++ " << p1.b << " ++ " << p1.cost); }
};

template <typename T>
double comp(T t1, T t2) { return t1 - t2; }
template <>
double comp(Edge e1, Edge e2) { return e1.cost - e2.cost; }


template <class T>
struct Heap {
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

	//T& getElementAt(int index) {
	//	if (index >= size || index < 0) {
	//		throw std::out_of_range("Zly indeks!");
	//	}
	//	return container[index];
	//}
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
	//T& operator[](int index) {
	//	return getElementAt(index);
	//}
};

template <class T>
struct dynArr {

	int size = 0;
	int capacity = 2;
	int multiplier = 2;

	T* currArr;
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

	dynArr() { currArr = new T[2]; }

	~dynArr() { delete[] currArr; }

	void addElement(T data) {
		if (size + 1 > capacity) {
			T* newArr = new T[capacity * multiplier];
			for (int i = 0; i < size; i++) { newArr[i] = currArr[i]; }

			size++;
			newArr[size - 1] = data; //o
			capacity = capacity * multiplier;
			delete[] currArr;
			currArr = newArr;
		}
		else {
			size++;
			currArr[size - 1] = data;
		}

	} //	A	

	T dataAt(int index) {
		if (index >= size || index < 0)
			throw std::out_of_range("Zly index!");

		else {
			return currArr[index];
		}

	} //	B

	void newDataAt(int index, T data) {
		if (index >= size || index < 0)
			throw std::out_of_range("Zly index!");

		else {
			currArr[index] = data;
		}

	} //	C
	void clearData() {
		T* newArr = new T[2];
		delete[] currArr;
		currArr = newArr;
		size = 0;
		capacity = 2;

	} //	D
	std::string toString(int howMany) { //string dla obiektow
		std::ostringstream out;
		out << "Tablica dynamiczna. \nObecny rozmiar: " << size << "\nObecna pojemnosc: " << capacity << std::endl;
		std::string output = out.str();
		out.str("");
		if (howMany > size || howMany < 0) { throw std::out_of_range("Niepoprawny indeks!"); }
		for (int i = 0; i < howMany; i++) {
			if (std::is_pointer<T>::value) { out << "Indeks: " << i << " Dane: " << printElement(currArr[i]) << std::endl; }
			else { out << "Indeks: " << i << " Dane: " << currArr[i] << std::endl; }
		}
		output = output + out.str();
		return output;
	}
	T& operator[](int index) {
		if (index >= size || index < 0) { throw std::out_of_range("Zly indeks!"); }
		else { return currArr[index]; }
	}
};

struct unionFind {
	dynArr<int> parent; //to sa indeksy obiektów-rodziców
	dynArr<int> rank; //to są górne ograniczenia na wysokości drzew
	int findCount = 0;

	unionFind(int nodes) {
		for (int i = 0; i < nodes; i++) { parent.addElement(i); } //początkowo są jednoelementowe zbiory 
		for (int i = 0; i < nodes; i++) { rank.addElement(0); } //początkowo każdy ma wysokość 0
	}
	int findNoPath(int find) {
		findCount++;
		if (parent[find] == find) { return parent[find]; }
		else { return findNoPath(parent[find]); }
	}
	int findWithPath(int find) {
		findCount++;
		if (parent[find] == find) { return parent[find]; }
		else {
			// if
			//parent[find] = findWithPath(parent[find]);
			int root = findWithPath(parent[find]);
			if (root != parent[find])
				parent[find] = root;
			return parent[find];
		} // optymalizacja z cachem ?? nie pamietam czy dodalem
	}
	void joinNoRank(int a, int b) {
		int a_root = findNoPath(a);
		int b_root = findNoPath(b);

		if (a_root != b_root) { parent[a_root] = b_root; }
	}
	void joinWithRank(int a, int b) {
		int a_root = findWithPath(a);
		int b_root = findWithPath(b);
		if (a_root != b_root) {
			if (rank[a_root] > rank[b_root]) {
				parent[b_root] = a_root;
			}
			else if (rank[a_root] < rank[b_root]) {
				parent[a_root] = b_root;
			}
			else {
				parent[b_root] = a_root;
				rank[a_root]++;
			}
		}

	}

};
void Kruskal(Heap<Edge>* edges, dynArr<Edge>* answer, int howManyNodes, int howManyEdges) {
	//KROK 1 : STRUKTURA UNION-FIND
	unionFind UF(howManyNodes);
	//KROK 2 : SORTOWANIE KRAWĘDZI ROSNĄCO
	auto t1 = std::chrono::high_resolution_clock::now();
	edges->fixHeap(0, comp);
	edges->sort(comp);
	auto t2 = std::chrono::high_resolution_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	//KROK 3. Dla każdej krawędzi(i, j) w posortowanej kolekcji 
	t1 = std::chrono::high_resolution_clock::now();


	int howManyFinds = 0;
	for (int i = 0; i < howManyEdges; i++) {
		Edge temp = edges->container[i];
		//3.1 Znajdź zbiór, do którego należy węzeł i(operacja find)
		//3.2 Znajdź zbiór, do którego należy węzeł j (operacja find).
		if (UF.findWithPath(temp.a) != UF.findWithPath(temp.b)) {
			//3.3 Jeżeli znalezione zbiory dla i oraz j są różne to: dodaj krawędź (i, j) do wynikowego drzewa
			//	rozpinającego oraz połącz te zbiory(operacja union).
			answer->addElement(temp);
			UF.joinWithRank(temp.a, temp.b);
			howManyFinds = howManyFinds + 4; //uznam że chodzi o "główne" findy, a nie te wołane przez same funkcje find
			if (answer->size == howManyNodes - 1) { break; }
		}
		else { howManyFinds = howManyFinds + 2; }
	}
	t2 = std::chrono::high_resolution_clock::now();
	auto timeTaken2 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	double sum = 0;
	for (int i = 0; i < answer->size; i++) {
		sum = sum + answer->currArr[i].cost;
	}
	std::cout << "Liczba krawedzi wynosi: " << answer->size << std::endl;
	std::cout << "Suma wag krawedzi w MST wynosi: " << sum << std::endl;
	std::cout << "Czas trwania operacji sortowania przez kopcowanie w mikrosekundach: " << timeTaken.count() << std::endl;
	std::cout << "Czas trwania glownej petli w algorytmie Kruskala: " << timeTaken2.count() << std::endl;
	std::cout << "Liczba wykonan operacji find w kruskalu( 2 + czasami 2 z uniona * liczba wykonan petli): " << howManyFinds << std::endl;
	std::cout << "Liczba wykonan operacji find w unionie( wszystkie ): " << UF.findCount << std::endl;

}
void Kruskal2(Heap<Edge>* edges, dynArr<Edge>* answer, int howManyNodes, int howManyEdges) {
	//KROK 1 : STRUKTURA UNION-FIND
	unionFind UF(howManyNodes);
	//KROK 2 : SORTOWANIE KRAWĘDZI ROSNĄCO
	auto t1 = std::chrono::high_resolution_clock::now();
	edges->fixHeap(0, comp);
	edges->sort(comp);
	auto t2 = std::chrono::high_resolution_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	//KROK 3. Dla każdej krawędzi(i, j) w posortowanej kolekcji 
	t1 = std::chrono::high_resolution_clock::now();


	int howManyFinds = 0;
	for (int i = 0; i < howManyEdges; i++) {
		Edge temp = edges->container[i];
		//3.1 Znajdź zbiór, do którego należy węzeł i(operacja find)
		//3.2 Znajdź zbiór, do którego należy węzeł j (operacja find).
		if (UF.findNoPath(temp.a) != UF.findNoPath(temp.b)) {
			//3.3 Jeżeli znalezione zbiory dla i oraz j są różne to: dodaj krawędź (i, j) do wynikowego drzewa
			//	rozpinającego oraz połącz te zbiory(operacja union).
			answer->addElement(temp);
			UF.joinNoRank(temp.a, temp.b);
			howManyFinds = howManyFinds + 4; //uznam że chodzi o "główne" findy, a nie te wołane przez same funkcje find
		}
		else { howManyFinds = howManyFinds + 2; }
	}
	t2 = std::chrono::high_resolution_clock::now();
	auto timeTaken2 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	double sum = 0;
	for (int i = 0; i < answer->size; i++) {
		sum = sum + answer->currArr[i].cost;
	}
	std::cout << "Liczba krawedzi wynosi: " << answer->size << std::endl;
	std::cout << "Suma wag krawedzi w MST wynosi: " << sum << std::endl;
	std::cout << "Czas trwania operacji sortowania przez kopcowanie w mikrosekundach: " << timeTaken.count() << std::endl;
	std::cout << "Czas trwania glownej petli w algorytmie Kruskala: " << timeTaken2.count() << std::endl;
	std::cout << "Liczba wykonan operacji find w kruskalu( 2 + czasami 2 z uniona * liczba wykonan petli): " << howManyFinds << std::endl;
	std::cout << "Liczba wykonan operacji find w unionie: " << UF.findCount << std::endl;

}

int main3213() {
	dynArr<Edge>* edges = new dynArr<Edge>();

	return 0;
}



int main() {
	Heap<Edge>* edges = new Heap<Edge>();
	dynArr<Edge>* answer = new dynArr<Edge>();
	std::ifstream currFile;
	std::string temp;
	std::string currNodes;
	std::string currEdges;
	currFile.open("g2.txt");
	if (currFile.is_open()) {
		std::getline(currFile, currNodes);
		for (int i = 0; i < std::stoi(currNodes); i++) { std::getline(currFile, temp); }
		temp.clear();
		std::getline(currFile, currEdges);
		for (int i = 0; i < std::stoi(currEdges); i++) { //ta pętla dzieli stringa z wartosciami krawedzi na czesci, i wpisuje do obiektu tymczasowego temp, nastepnie daje to tablicy edges
			std::getline(currFile, temp);
			std::istringstream temp1(temp);
			int a;
			int b;
			double c;
			temp1 >> a >> b >> c;
			Edge e1(a, b, c);
			edges->addElement(e1);
		}
	}
	currFile.close();
	Kruskal2(edges, answer, std::stoi(currNodes), std::stoi(currEdges));

	delete answer;
	delete edges;
	return 0;
}

int main313()
{
	srand(time(0));
	dynArr<int>* boxes = new dynArr<int>();
	const int MAX_ORDER = 3;
	const int n = pow(10, MAX_ORDER);
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < n; i++) {
			int a = rand() % 100;
			boxes->addElement(rand() % 10000);
		}
		std::cout << boxes->toString(999) << std::endl;
		boxes->clearData();


	}


	delete boxes;
	return 0;

}
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>	
#include <chrono>

struct Vector {

	double x;
	double y;

	Vector() {
		x = 0;
		y = 0;
	}
	Vector(double x, double y) {
		this->x = x;
		this->y = y;
	}
	friend std::ostream& operator<<(std::ostream& os, const Vector& v1) { return (os << v1.x << " ++ " << v1.y); }
};

struct Point {

	double x;
	double y;
	int index;
	Point() {
		x = 0;
		y = 0;
		index = 0;
	}

	Point(double x, double y, int index) {
		this->x = x;
		this->y = y;
		this->index = index;

	}
	friend std::ostream& operator<<(std::ostream& os, const Point& p1) { return (os << "X: " << p1.x << " Y: " << p1.y << " Indeks: " << p1.index); }

};

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

};
template <>
struct Heap<Point> {
	int size = 0;
	int capacity = 1;
	int multiplier = 4;
	Point* container;
	Point reference;


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

	Heap() { container = new Point[capacity]; }
	Heap(Point* container, int size, int (*cmp)(Point p0, Point p1, Point p2), bool variant) {
		this->container = container;
		this->size = size;
		capacity = size; //zakladamy ze tablica jest zapelniona
		fixHeap(variant, cmp);
	}
	~Heap() {
		delete[] container;
	}
	void addElement(Point data) {
		if (size == capacity) {
			Point* newContainer = new Point[capacity * multiplier];
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
		Point* newContainer = new Point[1];
		delete[] container;
		container = newContainer;
		size = 0;
		capacity = 1;
	}
	void deleteLast() {
		if (size > 0) {
			size--;
		}
	}
	int findIndex(Point data) {
		for (int i = 0; i < size; i++) {
			if (container[i].x == data.x && container[i].y == data.y) { return i; }
		}
		return 0;
	}

	std::string toString(int howMany) { //string dla obiektow
		std::ostringstream out;
		out << "Kopiec. \nObecny rozmiar: " << size << "\nObecna pojemnosc: " << capacity << std::endl;
		std::string output = out.str();
		out.str("");
		if (howMany > size || howMany < 0) { throw std::out_of_range("Niepoprawny indeks!"); }
		for (int i = 0; i < howMany; i++) {
			if (std::is_pointer<Point>::value) { out << "Indeks: " << i << " Dane: " << printElement(container[i]) << std::endl; }
			else { out << "Indeks: " << i << " Dane: " << container[i] << std::endl; }
		}
		output = output + out.str();
		return output;
	}


	int parent(int i) { return std::floor((i - 1) / 2); }
	int left(int i) { return 2 * i + 1; }
	int right(int i) { return 2 * i + 2; }

	void swapNodes(Point& A, Point& B) {
		Point temp = A;
		A = B;
		B = temp;
	}

	void heapUp(int i, int (*cmp)(Point p0, Point p1, Point p2)) {
		while (i > 0 && cmp(reference, container[parent(i)], container[i]) <= 0) {
			swapNodes(container[parent(i)], container[i]);
			i = parent(i);
		}
	}

	void heapDown(int i, int (*cmp)(Point p0, Point p1, Point p2)) {
		while (i < size) {
			int winnerIndex = i;
			if (left(i) < size && cmp(reference, container[winnerIndex], container[left(i)]) <= 0) { winnerIndex = left(i); }
			if (right(i) < size && cmp(reference, container[winnerIndex], container[right(i)]) <= 0) { winnerIndex = right(i); }

			if (winnerIndex != i) {
				swapNodes(container[i], container[winnerIndex]);
				i = winnerIndex;
			}
			else { break; }

		}

	}
	void addtoHeap(Point data, int (*cmp)(Point p0, Point p1, Point p2)) { //A
		addElement(data); //ETAP 1: umiesc element na samym koncu tablicy
		int i = size - 1;
		heapUp(i, cmp);		//ETAP 2: wykopcuj sie tak wysoko jak mozesz
	}
	Point pollRoot(int (*cmp)(Point p0, Point p1, Point p2)) {
		if (size == 0) { return Point(); }

		Point giveBack = container[0];
		swapNodes(container[0], container[size - 1]);
		size--;
		heapDown(0, cmp);

		return giveBack;

	}
	void fixHeap(bool variant, int (*cmp)(Point p0, Point p1, Point p2)) {
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


	void sort(int (*cmp)(Point p0, Point p1, Point p2)) {
		int rozmiar = size;
		while (size > 0) {
			swapNodes(container[0], container[size - 1]); //zamien roota z ostatnim elementem, ostatni element jest teraz na dobrej pozycji
			size--;
			heapDown(0, cmp);

		}
		size = rozmiar;
	}
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
	void deleteLast() {
		if (size > 0) {
			size--;
		}
	}

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
//(a.x - referencePoint.x, a.y - referencePoint.y, b.x - referencePoint.x, b.y - referencePoint.y) 
double crossProduct(Vector v1, Vector v2) {
	return -(v1.x * v2.y - v1.y * v2.x);
	//ad - bc
}
double distance(Point p0, Point p1) {
	double a = p1.x - p0.x;
	double b = p1.y - p0.y;
	return sqrt(pow(a, 2) + pow(b, 2));
}
bool collinear(Point p0, Point p1, Point p2) {
	double a = distance(p0, p1);
	double b = distance(p1, p2);
	double c = distance(p0, p2);
	return (a + b == c);

}


int comp(Point p0, Point p1, Point p2) {
	Vector p0p1(p1.x - p0.x, p1.y - p0.y);
	Vector p0p2(p2.x - p0.x, p2.y - p0.y);

	double cP = crossProduct(p0p1, p0p2);
	if (cP > 0) { return 1; }
	else if (cP < 0) { return -1; }
	else { //punkt mają ten sam kąt, są współliniowe
		double collinear = distance(p0, p1) - distance(p0, p2);
		if (collinear > 0) { return 1; }
		else { return -1; }
	}

}


//KROK 1 Znajdź punkt poczatkowy (skrajny lewy spośród najniższych) i potraktuj go jako nowy środek układu współrzędnych.
int findSmallestX(Heap<Point>* points) {
	int smallest = 0;
	for (int i = 1; i < points->size; i++) {
		if (points->container[i].y < points->container[smallest].y) { smallest = i; } //najmniejszy jest ten o najmniejszym y
		if (points->container[i].y == points->container[smallest].y) {	//jesli y sa takie same, najmniejszy jest ten o najmniejszym x
			if (points->container[i].x < points->container[smallest].x) { smallest = i; }
		}
	}
	return smallest; //zwracany jest indeks najmniejszego punktu
}

void grahamScan(Heap<Point>* points, dynArr<int>* convex_hull) {
	//KROK 1: Znajdź punkt poczatkowy (skrajny lewy spośród najniższych) i potraktuj go jako nowy środek układu współrzędnych.
	int smallest = findSmallestX(points); //indeks najmniejszego punktu


	Heap<Point>* points1 = new Heap<Point>;
	for (int i = 0; i < points->size; i++) {
		if (i != smallest) { points1->addElement(points->container[i]); }

	}

	//teraz mamy kopiec point1 bez poczatku ukladu wspolrzednych, ktory jest punktem odniesienia
	points1->reference = points->container[smallest];

	//KROK 2: Posortuj pozostałe punkty rosnąco według ich kąta względem wektora (1, 0).
	points1->fixHeap(0, comp); //rolę kluczową gra tu komparator
	points1->sort(comp);

	//KROK 3: Do powłoki wypukłej dodaj indeks punktu początkowego (nowy środek układu współrzędnych) oraz
	//pierwszego punktu z posortowanego porządku.
	Heap<Point>* points3 = new Heap<Point>;
	points3->addElement(points->container[smallest]);
	points3->addElement(points1->container[0]);

	int j = 1;
	//KROK 4: Przebiegaj pozostałe punkty w posortowanym porzadku (poczynając od drugiego):
	for (int i = 1; i < points1->size; i++) {
		//KROK 4.1: Dodaj indeks aktualnego punktu do powłoki wypukłej
		points3->addElement(points1->container[i]);
		j++;
		Vector v1(points3->container[j].x - points3->container[j - 1].x, points3->container[j].y - points3->container[j - 1].y);
		Vector v2(points3->container[j - 1].x - points3->container[j - 2].x, points3->container[j - 1].y - points3->container[j - 2].y);
		Point temp1(points3->container[j].x, points3->container[j].y, points3->container[j].index);
		while (crossProduct(v1, v2) < 0) {
			//4.2.1 Usuń indeks przedostatniego punktu z aktualnej powłoki wypukłej.
			points3->deleteLast();
			points3->deleteLast();
			points3->addElement(temp1);
			j = j - 1;
			v1.x = points3->container[j].x - points3->container[j - 1].x;
			v1.y = points3->container[j].y - points3->container[j - 1].y;
			v2.x = points3->container[j - 1].x - points3->container[j - 2].x;
			v2.y = points3->container[j - 1].y - points3->container[j - 2].y;
		}
	}
	std::cout << points3->toString(points3->size) << std::endl;

	delete points1;
	delete points3;
}
int main() {
	Heap<Point>* points = new Heap<Point>();
	dynArr<int>* convex_hull = new dynArr<int>();
	std::ifstream currFile;
	std::string currPoints;
	std::string temp;
	currFile.open("points2.txt");
	if (currFile.is_open()) {
		std::getline(currFile, currPoints);
		for (int i = 0; i < std::stoi(currPoints); i++) {
			std::getline(currFile, temp);
			std::istringstream temp1(temp);
			double x;
			double y;
			temp1 >> x >> y;
			Point p1(x, y, i);
			points->addElement(p1);
		}
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	grahamScan(points, convex_hull);
	auto t2 = std::chrono::high_resolution_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << "Czas trwania operacji algorytmu Grahama w milisekundach: " << timeTaken.count() << std::endl;

	delete points;
	delete convex_hull;
	return 0;
}




int main23() {
	Heap<Point>* points = new Heap<Point>();
	dynArr<int>* convex_hull = new dynArr<int>();
	std::ifstream currFile;
	std::string currPoints;
	std::string temp;
	currFile.open("points5.txt");
	if (currFile.is_open()) {
		std::getline(currFile, currPoints);
		for (int i = 0; i < std::stoi(currPoints); i++) {
			std::getline(currFile, temp);
			std::istringstream temp1(temp);
			double x;
			double y;
			temp1 >> x >> y;
			Point p1(x, y, i);
			points->addElement(p1);
		}
	}
	//KROK 1 i 3.11
	convex_hull->addElement(findSmallestX(points));


	std::cout << currPoints << std::endl;
	currFile.close();




	//KROK 2
	//points->fixHeap(0,comp);
	//points->sort(comp);
	//std::cout << points->toString(10) << std::endl;
	////KROK 3.12
	//convex_hull->addElement(0);
	////KROK 4

	//int j = 1;
	//for (int i = 1; i < points->size; i++) {
	//	//KROK 4.1
	//	convex_hull->addElement(i);
	//	j++;
	//	double v1 = comp(points->container[convex_hull->currArr[j - 1]], points->container[convex_hull->currArr[j - 2]]);
	//	double v2 = comp(points->container[convex_hull->currArr[j]], points->container[convex_hull->currArr[j - 1]]);
	//
	//	while (v1 - v2 < 0 && j >= 2) {
	//	 	convex_hull->deleteLast();
	//		convex_hull->deleteLast();
	//		j = j - 2;
	//		v1 = comp(points->container[convex_hull->currArr[j - 1]], points->container[convex_hull->currArr[j - 2]]);
	//		v2 = comp(points->container[convex_hull->currArr[j]], points->container[convex_hull->currArr[j - 1]]);
	//	}

	//}
	//std::cout << convex_hull->size << std::endl;
	//std::cout << convex_hull->toString(convex_hull->size - 1) << std::endl;
	////std::cout << convex_hull->toString(10) << std::endl;

	return 0;
}
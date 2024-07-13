#include <string>
#include <sstream>
#include <iostream>


//pamiêæ wiêksza o x 2.0
using namespace std;

struct Box {

	int daneA = 0;
	char daneB = 'c';
	string daneC = "Obywatel";
	Box() {}

	Box(int a, char b, string c) {
		daneA = a;
		daneB = b;
		daneC = c;

	}

	friend ostream& operator<<(ostream& os, const Box& b1) {
		return (os << b1.daneA << " " << b1.daneB << " " << b1.daneC);
	}
	friend ostream& operator<<(ostream& os, const Box* b1) {
		return (os << b1->daneA << " " << b1->daneB << " " << b1->daneC);
	}

};

template <class T>
bool comp(T t1, T t2) {
	return t1 < t2;
}

template <>
bool comp(Box t1, Box t2) {
	return t1.daneA < t2.daneA;
}
template <>
bool comp(Box* t1, Box* t2) {
	return t1->daneA < t2->daneA;
}
//nie chcia³o poprawnie sortowaæ bo by³ 'const'


template <class T>
class dynArr {
private:
	int size = 0;
	int capacity = 2;
	int multiplier = 2;

	T* currArr;
public:
	dynArr() {

		currArr = new T[capacity];

	}

	~dynArr() {
		delete[] currArr;
	}

	void addElement(T data) {
		if (size + 1 > capacity) {
			T* newArr = new T[capacity * multiplier];
			for (int i = 0; i < size; i++)
				newArr[i] = currArr[i];

			size++;
			newArr[size - 1] = data; //o Warning C6386  Buffer overrun: accessing 'buffer name', the writable size is 'size1' bytes, but 'size2' bytes may be written:
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
	string toString(int index) {
		ostringstream output;
		output << "Informacje:\nObecny rozmiar: " << size << "\nObecna pojemnosc: " << capacity << "\nDane:\n";
		string arr = output.str();
		output.str("");

		if (index > size || index < 0)
			throw std::out_of_range("Zly index!");

		for (int i = 0; i <= index; i++)
			output << "Indeks " << i << ". " << currArr[i] << " Adres: " << currArr + i * sizeof(T) << "\n";

		arr = arr + output.str();
		return arr;

	}

	string toString2(int index) {
		string txt = "";
		txt += "Informacje:\nObecny rozmiar: " + to_string(size) + "\nObecna pojemnosc: " + to_string(capacity) + "\nDane:\n";


		if (index > size || index < 0)
			throw std::out_of_range("Zly index!");

		for (int i = 0; i <= index; i++)
			txt += std::to_string((char)currArr[i]) + " Adres: " + std::to_string((int)&(currArr[i])) + "\n";

		return txt;

	}

	void bubbleSort(bool (*cmp)(T t1, T t2)) { //wskaŸnik na komparator jako argument

		bool flag;
		for (int i = 0; i < size - 1; i++) {
			flag = false;
			for (int j = 0; j < size - i - 1; j++) {
				if (cmp(currArr[j], currArr[j + 1])) {
					swap(currArr[j], currArr[j + 1]);
					flag = true;
				}
			}
			if (flag == false)
				break;
		}
	} //	E

};












#include <chrono>

int main111()
{
	dynArr<Box*>* boxes = new dynArr<Box*>();
	const int MAX_ORDER = 7;
	const int n = pow(10, MAX_ORDER);
	srand(time(0));
	auto t1 = std::chrono::high_resolution_clock::now();
	double maxTimePerElement = 0.0;
	for (int i = 0; i < n; i++) {
		auto tElement1 = std::chrono::high_resolution_clock::now();
		boxes->addElement(new Box(rand() % 100, char(rand() % 20 + 71), to_string(rand()%25 + 63)));
		auto tElement2 = std::chrono::high_resolution_clock::now();
		auto timeTakenForElement = std::chrono::duration_cast<std::chrono::microseconds>(tElement2 - tElement1);
		if (maxTimePerElement < timeTakenForElement.count()) {
			maxTimePerElement = timeTakenForElement.count();
			cout << "Nowy najgorszy czas dodawania.\nIndeks: " << i << endl;
			cout << "Czas wykonania operacji: " << timeTakenForElement.count() <<" mikrosekund" <<endl;
		}

	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto finalTime = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	cout << "Czas wszystkich operacji: " << finalTime.count() << " mikrosekund" << endl;
	cout << "Czas zamortyzowany: " << finalTime.count() / n << " mikrosekund" << endl;
	//boxes->bubbleSort(comp);
	cout << boxes->toString(9) << endl;


	boxes->clearData();
	delete boxes;

	return 0;
}
int main313() 
{
	srand(time(0));
	dynArr<Box*>* boxes = new dynArr<Box*>();
	const int MAX_ORDER = 3;
	const int n = pow(10, MAX_ORDER);
	for (int i = 0; i < n; i++) {
		int a = rand() % 100;
		boxes->addElement(new Box(int(a), char(rand() % 20 + 71), to_string(rand() % 25 + 63)));
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	boxes->bubbleSort(comp);
	auto t2 = std::chrono::high_resolution_clock::now();
	auto finalTime = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	cout << "Czas trwania sortowania b¹belkowego: " << finalTime.count() << " mikrosekund" << endl;
	cout << boxes->toString(999) << endl;
	auto box = boxes->dataAt(0);
	cout << *box << endl;
	boxes->newDataAt(0, new Box(99, char(rand() % 20 + 71), to_string(rand() % 25 + 63)));
	boxes->clearData();
	delete boxes;
	return 0;

}
int main()
{
	srand(time(0));
	dynArr<char>* boxes = new dynArr<char>();
	const int MAX_ORDER = 1;
	const int n = pow(10, MAX_ORDER);
	for (int i = 0; i < n; i++) {
		int a = rand() % 100;
		boxes->addElement(char(rand() % 20 + 'a'));
	}
	boxes->bubbleSort(comp);
	cout << boxes->toString(9) << endl;
	boxes->clearData();
	delete boxes;
	return 0;

}
int main4()
{
	srand(time(0));
	dynArr<int>* boxes = new dynArr<int>();
	const int MAX_ORDER = 3;
	const int n = pow(10, MAX_ORDER);
	for (int i = 0; i < n; i++) {
		int a = rand() % 100;
		boxes->addElement(rand() % 10000);
	}
	boxes->bubbleSort(comp);
	cout << boxes->toString(999) << endl;
	boxes->clearData();
	delete boxes;
	return 0;

}













	//dynArr<int>* a1 = new dynArr<int>();
	//dynArr<Box>* b1 = new dynArr<Box>();
	//dynArr<Box*>* b2 = new dynArr<Box*>();
	//dynArr<int*>* a2p = new dynArr<int*>();
	//int* a = new int;
	//int* b = new int;
	//a2p->addElement(a);
	//a2p->addElement(b);
	////cout << a2p->toString(1) << endl;
	//a1->addElement(31);
	//a1->addElement(32);
	//a1->addElement(33);
	//a1->addElement(34);
	//cout << a1->toString(3) << endl;
	//a1->bubbleSort(comp);
	//cout << a1->toString(3) << endl;
	//a1->clearData();
	//a1->addElement(35);
	//cout << a1->toString(0) << endl;

	//Box* b11 = new Box(5, 'a', "kys");
	//Box* b22 = new Box(13, 'b', "rly");
	//Box* b23 = new Box(7, 'c', "sponio");
	//Box* b24 = new Box(9, 'd', "forteen");
	//Box* b25 = new Box(0, 'a', "fr");
	//b2->addElement(b11);
	//b2->addElement(b22);
	//b2->addElement(b23);
	//b2->addElement(b24);
	//b2->addElement(b25);
	//cout << b2->toString(4) << endl;
	//b2->bubbleSort(comp);
	//cout << b2->toString(4) << endl;

	//delete a;
	//delete b;


	//b1->addElement(Box(2, 'c', "hi"));
	//b1->addElement(Box(3, 'd', "bye"));
	//cout << b1->toString(1) << endl;


	/*a1->addElement(3);
	a1->addElement(4);
	a1->addElement(5);
	a1->addElement(6);*/
	//cout << "Dane w 0: " << a1->dataAt(0) << endl;
	//cout << "Dane w 1: " << a1->dataAt(1) << endl;
	//cout<< "Dane w 2: "<< a1->dataAt(2) << endl;
	//cout << "Dane w 3: " << a1->dataAt(3) << endl;
	//a1->newDataAt(2, 11);
	//cout << a1->dataAt(2) << endl;

	//cout << a1->toString(2) << endl;
	//a1->clearData();
	//a1->~dynArr();


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

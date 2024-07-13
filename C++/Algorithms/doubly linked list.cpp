#include <string>
#include <iostream>
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
};





template <class T>
string makeString(const T& t1) {

	return to_string((int)t1);
}


template <>
string makeString(const Box& b1) {
	return to_string(b1.daneA) + " " + b1.daneB + " " + b1.daneC;
}

template <class T>
bool comp(const T& t1, const T& t2, int flag) {
	if (flag == 0)
		return t1 < t2;

	else if (flag == 1)
		return t1 > t2;

	else {
		return t1 == t2;
	}
}
template <>
bool comp(const Box& p1, const Box& p2, int flag) {
	if (flag == 0)
		return p1.daneA < p2.daneA;
	else if (flag == 1)
		return p1.daneA > p2.daneA;

	else {
		return p1.daneA == p2.daneA;
	}
}




template <class T>
struct Node {
	Node<T>* prevNode{ nullptr };
	Node<T>* nextNode{ nullptr };

	T data;


	Node() {
		//data = "0";
	}

	Node(T data) {
		this->data = data;
	}
	~Node() {}


};
/*
template <class T>
struct Node<T*> {
	Node<T>* prevNode{ nullptr };
	Node<T>* nextNode{ nullptr };

	T* data;

	Node() {
		data = "0";
	}


	Node(T* data) {
		this->data = new T{ *data };
	}

	~Node(){
		delete data;
	}


};
*/
template <class T>
class List {
private:
	int size = 0;
	Node<T>* head = { nullptr };
	Node<T>* tail = { nullptr };

public:
	List() {};

	List(T data) {

		head = new Node<T>(data);
		tail = head;
		size++;

	}

	~List() {
		while (head) {
			Node<T>* temp = head;
			head = head->nextNode;
			delete temp;
			size--;
		}
	}

	void print() {
		if (size == 0) {
			cout << "NULL" << endl;
			cout << "Size = " << size << endl;
			return;
		}

		Node<T>* curr = head;
		while (curr) {
			cout << curr->data << " <-> ";
			curr = curr->nextNode;
		}
		cout << "NULL" << endl;
		cout << "Size = " << size << endl;
	} //pomocnicza

	void addEnd(const T data) {
		Node<T>* toAdd = new Node<T>(data);
		if (size == 0) {
			tail = toAdd;
			head = tail;
			size++;
			return;
		}

		toAdd->prevNode = tail;
		tail->nextNode = toAdd;
		tail = toAdd;
		size++;
	} // A

	void addBeg(T data) { //a
		Node<T>* toAdd = new Node<T>(data);

		if (size == 0) {
			head = toAdd;
			tail = head;
			size++;
			return;
		}

		toAdd->nextNode = head;
		head->prevNode = toAdd;
		head = toAdd;
		size++;
	} // B

	void delEnd() {
		if (size == 0)
			return;
		else if (size == 1) {
			delete tail;
			size--;
			//return;
		}
		else {
			Node<T>* temp = tail->prevNode;
			tail->prevNode->nextNode = nullptr;
			//tail->prevNode = nullptr;
			delete tail;
			tail = temp;
			size--;
		}
	} // C

	void delBeg() {
		if (size == 0)
			return;
		else if (size == 1) {
			delete head;
			size--;
			//return
		}
		else {
			Node<T>* temp = head->nextNode;
			head->nextNode->prevNode = nullptr;
			head->nextNode = nullptr;
			delete head;
			head = temp;
			size--;
		}
	} // D

	T* dataAt(const int index) {
		if (index > size - 1 || index < 0) {
			//cout << "Zwracam nullptr!" << endl; 
			throw std::out_of_range("dsasa");

			return nullptr;
		}
		else {
			Node<T>* curr = head;
			for (int i = 0; i < index; i++) {
				curr = curr->nextNode;
			}
			T* found = &(curr->data);
			return found;
		}
	} // E

	void changeDataAt(const int index, const T& newData) {
		if (index > size - 1 || index < 0) {
			cout << "Indeks poza zasiegiem!" << endl;
			return;
		}
		else {
			Node<T>* curr = head;
			for (int i = 0; i < index; i++) {
				curr = curr->nextNode;
			}
			curr->data = newData;
		}
	} // F
	Node<T>* findData(const T& compareTo, const int flag) {  //czyli np. znajdź pierwszą daną, której pierwsze pole jest równe pierwszemu polu data, i jej drugie pole jest większe od x
		if (size < 1)   // flag 0: a < b, flag 1: a > b, flag inne: a == b dla typów podstawowych
			return nullptr;


		Node<T>* curr = head;
		while (curr) {
			if (comp(compareTo, curr->data, flag))
				return curr;
			else {
				curr = curr->nextNode;
			}
		}
		return nullptr;

	} // G 
	bool searchDel(const T& compareTo, const int flag) {
		if (size < 1)
			return 0;

		Node<T>* curr = head;
		while (curr) {
			if (comp(compareTo, curr->data, flag)) {
				if (curr == head) {
					cout << "usuwam poczatek" << endl;
					delBeg();
					return 1;
				}
				else if (curr == tail) {
					cout << "usuwam koniec" << endl;
					delEnd();
					return 1;
				}

				else {
					cout << "usuwam srodek" << endl;
					curr->prevNode->nextNode = curr->nextNode;
					curr->nextNode->prevNode = curr->prevNode;
					curr->nextNode = nullptr;
					curr->prevNode = nullptr;
					delete curr;
					size--;
					return 1;
				}


			}
			else {
				curr = curr->nextNode;
			}

		}
		return 0;


	} // H

	void searchAdd(const T& compareTo, const int flag) {
		if (size < 1) {
			addBeg(compareTo);
			return;
		}
		Node<T>* curr = head;
		while (curr) {
			if (comp(compareTo, curr->data, flag)) {
				if (curr == head) {
					addBeg(compareTo);
					return;
				}
				else {
					Node<T>* toAdd = new Node<T>(compareTo);
					toAdd->nextNode = curr;
					toAdd->prevNode = curr->prevNode;
					curr->prevNode->nextNode = toAdd;
					curr->prevNode = toAdd;
					size++;
					return;
				}
			}
			else {
				curr = curr->nextNode;
			}
		}


	} //I węzeł będzie dodawany przed węzłem spełniającym warunek wyszukiwania; czy obiekt podany jako argument jest mniejszy wiekszy równy od obiektu w nodzie

	void clearList() {
		//	Node<T>* curr = head;
		while (size > 0) {
			delBeg();
		}
	} //J działa

//	string proba() {
//		return makeString(head->data);
//	}


	string toString(int index) {
		string list = "";


		if (index < 0) {
			cout << "Nieprawidlowy indeks!" << endl;
			return  list;
		}
		else if (index > size - 1) {
			return toString(0);
		}
		else {
			string address = std::to_string((uintptr_t)(void**)head); //konwersja adresu na string
			list.append("Rozmiar listy: " + to_string(size) + "\n" + "Adres listy: " + address + "\n" + "Dane listy: " + "\n");

			Node<T>* curr = head;
			for (int i = 0; i <= index; i++) {
				T* data = &(curr->data);
				if (is_pointer<T>::value) {
					list.append("Indeks: " + to_string(i) + " Dane: " + makeString(*data) + "\n"); //To tutaj nie działa
				}
				else {
					list.append("Indeks: " + to_string(i) + " Dane: " + makeString(curr->data) + "\n");
				}
				curr = curr->nextNode;
			}
		}

		return list;

	} //K



	int getSize() {
		return size;
	}

	Node<T>*& getHead() {
		return head;
	}
	Node<T>*& getTail() {
		return tail;
	}
};





#include <chrono>


int main1() {
	int* a = new int;
	*a = 5;
	int* b = new int;
	*b = 6;

	List < int >* l1 = new List < int >(); //ta lista przechowuje wartości
	List < Box >* l1Box = new List < Box >();
	List < int* >* l3 = new List < int* >(a); //ta lista przechowuje w węzłach wskaźniki na wartości
	//l2->addEnd(5, 6); //z tego będę korzystał
	const int MAX_ORDER = 6;
	srand(time(0));
	for (int i = 0; i <= MAX_ORDER; i++) {
		const int n = pow(10, i);
		l1->clearList();
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int j = 0; j < n; j++) {
			int numer = 0;
			//l1Box->addBeg()
			numer = rand() % 9999;
			l1->addEnd(numer);
			//l3->addEnd(numer);
			
		}

		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		cout <<"Czas trwania operacji w mikrosekundach: " << timeTaken.count() << endl;
		cout << l1->toString(9) << endl;
		//cout << l3->toString(10) << endl; to nie działa
	
	}


	return 0;


}

int main2() {
	List < Box >* lBox = new List < Box >();
	const int MAX_ORDER = 6;
	srand(time(0));
	for (int i = 0; i <= MAX_ORDER; i++) {
		const int n = pow(10, i);
		lBox->clearList();
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int j = 0; j < n; j++) {
			int a = rand() % 9999;
			char b = char(rand() % 20 + 71);
			string c = "w" + to_string(rand() % 999);
			Box dana = Box(a, b, c);
			lBox->addEnd(dana);
					
		}
	
		Box toFind = Box(7, 'D', "w101");
		auto t3 = std::chrono::high_resolution_clock::now();
		auto find = lBox->findData(toFind, 2);
		auto t4 = std::chrono::high_resolution_clock::now();
		auto timeTaken1 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
		cout << "Czas trwania operacji wyszukiwania w mikrosekundach: " << timeTaken1.count() << endl;
		if (find) {
			cout << find << endl;
			auto z = *find;
			cout << z.data.daneA << endl;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		cout << "Czas trwania operacji w mikrosekundach: " << timeTaken.count() << endl;
		cout << lBox->toString(9) << endl;
		//cout << l3->toString(10) << endl; to nie działa





	}
	
	
	
	
	return 0;
}



int main() {
	int* numer = new int;
	*numer = 5;
	List < int* >* l3 = new List < int* >(numer); //ta lista przechowuje w węzłach wskaźniki na wartości
	int* numer1 = new int;
	*numer1 = 6;
	l3->addBeg(numer1);
	cout << l3->toString(1) << endl;


	return 0;
}












/*	l2->addEnd(1);
	l2->print();
	l2->addBeg(7);
	l2->print();
	l2->delBeg();
	l2->delBeg();
	l2->print();
	l2->delBeg();
	l2->print();
	l2->addBeg(1);
	l2->print();
	l2->addBeg(3);
	l2->addBeg(6);
	l2->print();

	//F
	l2->changeDataAt(1, 41);
	l2->print();
	int find = 2;

	//E
	auto x = l2->dataAt(0);
	cout << *x << endl;
	// 0 : a < b, 1: a > b, 2: a == b
	auto y = l2->findData(find, 1);
	cout << y << endl;
	cout << "jestesmy tu" << endl;
	l2->print();
	cout<< l2->searchDel(2, 1) << endl;
	cout << "po usuwaniu" << endl;
	l2->print();
	l2->searchAdd(5, 0);
	l2->print();
	cout << "Sekundy przed tragedia" << endl;
	l2->clearList(); //po clearze nic nie ma
	cout << "Po tragedii" << endl;
	l2->addBeg(7);
	l2->print();
	l2->addEnd(5);
	l2->addBeg(3);
	l2->print();
	cout << l2->toString(2) << endl;

	l2->addEnd(9);
	cout << l2->toString(3) << endl;
	l2->changeDataAt(3, 21);
	cout << l2->toString(3) << endl;
	//cout << *c << endl;
	//l3->addEnd(a, b);
	//l3->addEnd(a, b); //z tego nie chcę korzystać
	return 0;

	//l1.print();
	//l2->print();
	//l3->print();*/
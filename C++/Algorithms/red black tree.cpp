#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>




//1. każdy węzeł ma kolor(czerwony lub czarny),
//2. korzeń jest czarny,
//3. każdy liść jest czarny(przy czym jako liście nie są traktowane najniższe węzły, zaś puste wskaźniki
//	 potomne odchodzące od nich — tzw.NULLe lub NILe),
//4. czerwony węzeł musi mieć czarne dzieci,
//5. każda ścieżka z dowolnego ustalonego węzła do dowolnego osiągalnego liścia ma tyle samo czarnych
//	węzłów

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
	friend std::ostream& operator<<(std::ostream& os, const Pudlo& p1) { return (os << p1.key << " " << p1.dane); }
	friend std::ostream& operator<<(std::ostream& os, const Pudlo*& p1) { return (os << p1->key << " " << p1->dane); }
};

struct Box {

	int daneA = 0;
	char daneB = 'c';
	Box() {}

	Box(int a, char b) {
		daneA = a;
		daneB = b;
	}

	friend std::ostream& operator<<(std::ostream& os, const Box& b1) {
		return (os << b1.daneA << " " << b1.daneB);
	}
	friend std::ostream& operator<<(std::ostream& os, const Box* b1) {
		return (os << b1->daneA << " " << b1->daneB);
	}
};





template<class T>
struct Node {
	Node<T>* parent = nullptr;
	Node<T>* leftChild = nullptr;
	Node<T>* rightChild = nullptr;

	int id;
	static int UID;
	T data{};
	bool colour = 1;	//0 - czerwony, 1 - czarny
	bool NIL = 1; //0 - faktyczny wezel, 1 - NIL

	Node() { id = -1; };
	Node(T someData) {
		data = someData;
		id = UID++;
		NIL = 0;
	}
	~Node() {}
};
template <class T>
int Node<T>::UID = 0;

template<class T>
class RBTree {
private:
	Node<T>* root = nullptr;
	int size = 0;
public:
	RBTree() {}
	~RBTree() { if (root) { clearTree(root); } }
//-------------------------------------------------------------------------
	Node<T>* findNode(T key, int (*cmp)(T t1, T t2)) {			//A
		Node<T>* klucz = new Node<T>(key);
		int compare = 0;
		Node<T>* currNode = root;
		while (currNode->NIL == 0) {
			compare = cmp(klucz->data, currNode->data);
			if (compare == 0) {
				delete klucz;
				return currNode;
			}
			else if (compare < 0) { currNode = currNode->leftChild; }
			else { currNode = currNode->rightChild; } 
		}
		delete klucz;
		return nullptr;
	}
//-------------------------------------------------------------------------
	void preOrderTraversal(Node<T>* currNode, std::vector<T>*& values) { //B
		if (currNode == nullptr) { return; }
		values->push_back(currNode->data);
		preOrderTraversal(currNode->leftChild, values);
		preOrderTraversal(currNode->rightChild, values);
	}
	void preOrderTraversal(std::vector<T>*& values) { preOrderTraversal(root, values); }
//-------------------------------------------------------------------------
	void inOrderTraversal(Node<T>* currNode, std::vector<T>*& values) {  //C
		if (currNode == nullptr) { return; }
		inOrderTraversal(currNode->leftChild, values);
		values->push_back(currNode->data);
		inOrderTraversal(currNode->rightChild, values);
	}
	void inOrderTraversal(std::vector<T>*& values) { inOrderTraversal(root, values); }
//-------------------------------------------------------------------------
	void clearTree(Node<T>*& currNode) {									//D
		if (currNode == nullptr) { return; }
		clearTree(currNode->leftChild);
		clearTree(currNode->rightChild);
//-------------------------------------- usuwania wezla bez dzieci
		if (currNode->parent != nullptr) {
			if (currNode == currNode->parent->leftChild) { currNode->parent->leftChild = nullptr; }
			else { currNode->parent->rightChild = nullptr; } 
		}
		else { root = nullptr; } 

		delete currNode;
		size--;
		if (size < 10) { std::cout << size << std::endl; }
	}
	void clearTree() { clearTree(root); }
//-------------------------------------------------------------------------
	int treeHeight(Node<T>* &currNode) {									//E
		if (currNode == nullptr) { return 0; }
		else {
			int leftChild = treeHeight(currNode->leftChild);
			int rightChild = treeHeight(currNode->rightChild);
			
			if (leftChild >= rightChild) { return leftChild + 1; }
			else { return rightChild + 1; }
		}

	}
	int treeHeight() { return treeHeight(root); }

//-------------------------------------------------------------------------
	Node<T>* addNodeNEW(T data, int (*cmp)(T t1, T t2)) {					//F ETAP-1
		Node<T>* toAdd = new Node<T>(data);
		Node<T>* NIL_1 = new Node<T>();
		Node<T>* NIL_2 = new Node<T>();

		//Node<T>* parentNode = nullptr;
		Node<T>* currNode = nullptr;
		if (root == nullptr) {		//	JESTEŚMY ROOTEM, DODAJ 3 NODY
			root = toAdd;

			NIL_1->parent = root;
			root->leftChild = NIL_1;

			NIL_2->parent = root;
			root->rightChild = NIL_2;



			size = size + 3;
			return toAdd;
		}
		else {
			currNode = root;
			int compare = 0;
			while (currNode->NIL == 0) {
			//	parentNode = currNode;
				compare = cmp(toAdd->data, currNode->data);
				if (compare <= 0) { currNode = currNode->leftChild; }
				else { currNode = currNode->rightChild; }
			}
			toAdd->parent = currNode->parent;
			if (currNode == currNode->parent->leftChild) { currNode->parent->leftChild = toAdd; }
			else { currNode->parent->rightChild = toAdd; }
			delete currNode; //usuwamy NILA - to byl placeholder



			toAdd->leftChild = NIL_1;
			NIL_1->parent = toAdd;
			toAdd->rightChild = NIL_2;
			NIL_2->parent = toAdd;

			//toAdd->parent = parentNode;
			//if (compare <= 0) { parentNode->leftChild = toAdd; }
			//else { parentNode->leftChild = toAdd; }
			size = size + 2;
			return toAdd;
		}
	}
//-------------------------------------------------------------------------	
	void addNodeRBT_final(T data, int (*cmp)(T t1, T t2)) {					//F ETAP-2
		Node<T>* currNode = addNodeNEW(data, cmp);
		Node<T>* uncle = nullptr;
		Node<T>* grandfather = nullptr;
		Node<T>* father = nullptr;
		currNode->colour = 0;
		while (currNode != root && currNode->parent->colour == 0) {
//-------------------------------------------------------------------------
			if (currNode->parent->parent) { grandfather = currNode->parent->parent; } //ojciec jest po lewej dziadka
			father = currNode->parent;
			if (grandfather && father == grandfather->leftChild) {
				uncle = grandfather->rightChild;
				if (uncle->colour == 0) {	//stryj jest czerwony (KOLORUJEMY)
					father->colour = 1;
					uncle->colour = 1;
					grandfather->colour = 0;
					currNode = grandfather;
				}
				else {						//stryj jest czarny (KOLORUJEMY + ROTACJE)
					if (currNode == father->rightChild) {	//dodatkowo jest lamana (dwie rotacje zamiast jednej)
						leftRotate_final(currNode);			//ROTACJA AWANSUJE WEZEL NA NIZSZE PIETRO
						father = currNode;
					}
					father->colour = 1;
					grandfather->colour = 0;
					rightRotate_final(father); 
				}

			}
//------------------------------------------------------------------------	//ojciec jest po prawej stronie dziadka
			else if(grandfather && father == grandfather->rightChild) {
				uncle = grandfather->leftChild; //stryj czerwony (KOLORUJEMY)
				if (uncle->colour == 0) {
					father->colour = 1;
					uncle->colour = 1;
					grandfather->colour = 0;
					currNode = grandfather;
				}
				else {						//stryj czarny (KOLORUJEMY + ROTACJE)
					if (currNode == father->leftChild) {	//dodatkowo jest lamana (dwie rotacje zamiast jednej)
						rightRotate_final(currNode);		//ROTACJA AWANSUJE WEZEL NA NIZSZE PIETRO
						father = currNode;
					}
					father->colour = 1;
					grandfather->colour = 0;
					leftRotate_final(father);
				}
			}
		}
		root->colour = 1;
	}
	void leftRotate_final(Node<T>* node) { //rotacje nadaja wezlowi nizszy poziom
		Node<T>* temp = nullptr;
		if (node == root) {					//jestesmy korzeniem i chcemy zrobic rotacje (moze sie przyda)
			if (node->rightChild->leftChild) { temp = node->rightChild->leftChild; } //te ify sa niepotrzebne
			root->parent = root->rightChild;
			root->parent->leftChild = root;
			if (temp) {
				root->rightChild = temp;
				temp->parent = root;
			}

			root->parent->parent = nullptr;
			root = root->parent;
		}
		else if (node->parent == root) { //jestesmy synem korzenia, wiec nie ma dziadka, praktycznie to samo co wyzej ale chcialem sobie rozpisac
			node = root;				 //zmieniamy perspektywe na roota
			if (node->rightChild->leftChild) { temp = node->rightChild->leftChild; } //te ify sa niepotrzebne bo dodawanie wezlow gwarantuje nam wszedzie wezly NIL
			root->parent = root->rightChild;
			root->parent->leftChild = root;
			if (temp) {
				root->rightChild = temp;
				temp->parent = root;
			}

			root->parent->parent = nullptr;
			root = root->parent;
		}
		else {	//jestesmy glebiej czyli dziadek istnieje
			if (node->leftChild) { temp = node->leftChild; }
			node->leftChild = node->parent;
			node->parent = node->parent->parent;
			if (node->leftChild == node->parent->leftChild) { node->parent->leftChild = node; }		//ustalamy ktorym dzieckiem byl wczesniejszy rodzic
			else { node->parent->rightChild = node; }
			node->leftChild->parent = node;
			if (temp) {
				node->leftChild->rightChild = temp;
				temp->parent = node->leftChild;
			}
		}
	}
	void rightRotate_final(Node<T>* node) {
		Node<T>* temp = nullptr;
		if (node == root) {					//jestesmy korzeniem i chcemy zrobic rotacje (moze sie przyda)
			if (node->leftChild->rightChild) { temp = node->leftChild->rightChild; } //te ify sa niepotrzebne
			root->parent = root->leftChild;
			root->parent->rightChild = root;
			if (temp) { 
				root->leftChild = temp; 
				temp->parent = root;
			}
			
			root->parent->parent = nullptr;
			root = root->parent;
		}
		else if (node->parent == root) { //jestesmy synem korzenia, wiec nie ma dziadka, praktycznie to samo co wyzej ale chcialem sobie rozpisac
			node = root;				 //zmieniamy perspektywe na roota
			if (node->leftChild->rightChild) { temp = node->leftChild->rightChild; } //te ify sa niepotrzebne
			root->parent = root->leftChild;
			root->parent->rightChild = root;
			if (temp) {
				root->leftChild = temp;
				temp->parent = root;
			} 

			root->parent->parent = nullptr;
			root = root->parent;
		}
		else {	//jestesmy glebiej czyli dziadek istnieje
			if (node->rightChild) { temp = node->rightChild; }
			node->rightChild = node->parent; 
			node->parent = node->parent->parent;
			if (node->rightChild == node->parent->rightChild) { node->parent->rightChild = node; }		//ustalamy ktorym dzieckiem byl wczesniejszy rodzic
			else { node->parent->leftChild = node; }
			node->rightChild->parent = node;
			if (temp) {
				node->rightChild->leftChild = temp;
				temp->parent = node->rightChild;
			}
		}
	}

	std::string printNode(Node<T>* currNode) {
		std::ostringstream nodeOutput;
		if (!currNode) { return ""; }
		else {
			std::string parent = "NULL";
			std::string leftChild = "NULL";
			std::string rightChild = "NULL";
			if (currNode->parent) { parent = std::to_string(currNode->parent->id); }
			if (currNode->leftChild) { leftChild = std::to_string(currNode->leftChild->id); }
			if (currNode->rightChild) { rightChild = std::to_string(currNode->rightChild->id); }

			//działa
			nodeOutput << "(ID: " << currNode->id << ": [" << "p: " << parent << ", " << "l: " << leftChild << ", " << "r: " << rightChild << "], " << "data: " << currNode->data << " kolor: " << currNode->colour << ")\n";
			std::string arr = nodeOutput.str();
			//nodeOutput.str("");
			return arr;
		}

	}
//-------------------------------------------------------------------------
	void preOrderTraversalString(Node<T>* currNode, std::string& temp, int& nodeCount) { //root->lewe poddrzewo po kolei -> prawe poddrzewo (WAŻNE!!!) musisz przekazać stringa przez referencję inaczej nie będzie działać
		if (!currNode) { return; }
		if (nodeCount <= 0) { return; } 
		temp = temp + printNode(currNode);
		preOrderTraversalString(currNode->leftChild, temp, nodeCount = nodeCount - 1);
		preOrderTraversalString(currNode->rightChild, temp, nodeCount = nodeCount - 1);
	}
//-------------------------------------------------------------------------
	std::string toString() {
		std::ostringstream output;
		output << "Drzewo czerwono - czarne:\nObecny rozmiar: " << size / 2 << ". Jest: " << size/2 + 1 << " NIL wezlow. " << "\nObecna wysokosc: " << treeHeight() << "\nDane:\n";
		std::string arr = output.str();
		std::string temp = "";
		int nodeCount = 200;
		preOrderTraversalString(root, temp, nodeCount);
		arr = arr + temp;
		return arr;
	}
//-------------------------------------------------------------------------
};





template<typename T>
int comp(T t1, T t2) { return t1 - t2; }
template<>
int comp(Pudlo p1, Pudlo p2) { return p1.key - p2.key; }
template<typename T>
int comp(T* t1, T* t2) { return *(t1) - *(t2); } //ostatnie zmiany
template<>
int comp(Pudlo* p1, Pudlo* p2) { return p1->key - p2->key; }



int mainTEST() {
	return 0;
}
//wskazniki na pudla
int main()
{
	srand(time(NULL));
	const int MAX_ORDER = 7; // maksymalny rzad wielkosci dodawanych danych do 7 jest stack overflow
	RBTree < Pudlo* >* rbt = new RBTree < Pudlo* >(); // stworzenie drzewa
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		// dodawanie do drzewa
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			int val = (rand() << 15) + rand();
			Pudlo* temp = new Pudlo(val, 'A');
			rbt->addNodeRBT_final(temp, comp);
			//rbt->rb_insert(add, comp);
			delete temp;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		//std::cout << rbt->toString() << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl; // wypis na ekran aktualnej postaci drzewa ( skrotowej ) wraz z wysokosciaoraz pomiarow czasowych
		std::cout <<  "Stosunek wysokosci drzewa do (2*logarytmu2(n) + 1) : "  << (rbt->treeHeight() - 1) / (2 * log2(n) + 1) << std::endl;
		// wyszukiwanie
		const int m = pow(10, 4); // liczba prob wyszukiwania
		int hits = 0; // liczba trafien
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < m; i++)
		{
			int val = (rand() << 15) + rand();
			Pudlo* temp = new Pudlo(val, 'B');
			Node<Pudlo*>* result = rbt->findNode(temp, comp); //zwraca wskaznik na wezel przechowujacy wskaznik do danych //cos tu trzeba rozparcelowac
			//if (result) { std::cout << result << *(result)->data <<std::endl; } //wydruk zwraca wskazniki na tempy ktore sa wzorcem do szukania innych wskaznikow, zamiast zwracac wskazniki
			if (result && result->NIL == 0) { hits++; }
			//if (rbt->findNode(temp, comp)) {
			//	auto result = rbt->findNode(temp, comp);
			//	if (result) { hits++; }
			//}
			delete temp;
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Liczba trafien: " << hits << std::endl;
		std::cout << "Czas trwania operacji wyszukiwania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji wyszukiwania w mikrosekundach: " << timeTaken.count() / m << std::endl; // wypis pomiarow i ilosci trafien
		rbt->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych

	}
	delete rbt;
	return 0;
}


//PUDLA
int main1889()
{
	srand(time(NULL));
	const int MAX_ORDER = 7; // maksymalny rzad wielkosci dodawanych danych do 7 jest stack overflow
	RBTree < Pudlo >* rbt = new RBTree < Pudlo > (); // stworzenie drzewa
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		// dodawanie do drzewa
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			int val = (rand() << 15) + rand();
			Pudlo temp(val, 'a');
			rbt->addNodeRBT_final(temp, comp);
			//rbt->rb_insert(add, comp);
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << rbt->toString() << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl; // wypis na ekran aktualnej postaci drzewa ( skrotowej ) wraz z wysokosciaoraz pomiarow czasowych
		std::cout <<  "Stosunek wysokosci drzewa do (2*logarytmu2(n) + 1) : "  << (rbt->treeHeight() - 1) / (2*log2(n) + 1) << std::endl;
		// wyszukiwanie
		const int m = pow(10, 4); // liczba prob wyszukiwania
		int hits = 0; // liczba trafien
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < m; i++)
		{
			int val = (rand() << 15) + rand();
			Pudlo temp(val, 'a');
			auto result = rbt->findNode(temp, comp);
			if (result && result->NIL == 0) { hits++; }

		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Liczba trafien: " << hits << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / m << std::endl; // wypis pomiarow i ilosci trafien
		rbt->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych

	}
	delete rbt;
	return 0;
}
//INTY
int main1()
{
	srand(time(NULL));
	const int MAX_ORDER = 7; // maksymalny rzad wielkosci dodawanych danych do 7 jest stack overflow
	RBTree < int >* rbt = new RBTree < int >(); // stworzenie drzewa
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		// dodawanie do drzewa
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)
		{
			int val = (rand() << 15) + rand();
			rbt->addNodeRBT_final(val, comp); // dodanie ( drugi argument to wskaznik na komparator )
			//std::cout << "Wykonalem dodawanie!" << std::endl;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << rbt->toString() << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl; // wypis na ekran aktualnej postaci drzewa ( skrotowej ) wraz z wysokosciaoraz pomiarow czasowych
		std::cout << "Stosunek wysokosci drzewa do (2*logarytmu2(n) + 1) : " << rbt->treeHeight() / (2*log2(n) + 1) << std::endl;
		// wyszukiwanie
		const int m = pow(10, 4); // liczba prob wyszukiwania
		int hits = 0; // liczba trafien
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < m; i++)
		{
			int val = (rand() << 15) + rand();
			auto result = rbt->findNode(val, comp);
			if (result && result->NIL == 0) { hits++; }
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Liczba trafien: " << hits << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / m << std::endl; // wypis pomiarow i ilosci trafien
			rbt->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych
			std::cout << "KASUJE DRZEWO" << std::endl;
	
	}
	delete rbt;
	return 0;
}

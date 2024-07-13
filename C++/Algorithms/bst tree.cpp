//ALGO2 IS1 210B LAB03
//Maksymilian Grobicki - Madej
//gm49261@zut.edu.pl


#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
struct Box {

	int klucz = 0;
	char daneB = 'c';
	std::string daneC = "Obywatel";
	Box() {}

	Box(int a, char b, std::string c) {
		klucz = a;
		daneB = b;
		daneC = c;

	}
	~Box() {}
	friend std::ostream& operator<<(std::ostream& os, const Box& b1) {
		return (os << b1.klucz << " " << b1.daneB << " " << b1.daneC);
	}
	friend std::ostream& operator<<(std::ostream& os, const Box* b1) {
		return (os << b1->klucz << " " << b1->daneB << " " << b1->daneC);
	}
};

template<class T>
int comp(T t1, T t2) {
	return t1 - t2;
	//if (t1 < t2)
	//	return -1;
	//else if (t1 == t2)
	//	return 0;
	//else
	//	return 1;
}

template <>
int comp(Box t1, Box t2) {
	if (t1.klucz < t2.klucz)
		return -1;
	else if (t1.klucz == t2.klucz)
		return 0;
	else
		return 1;
}
template <>
int comp(Box* t1, Box* t2) {
	if (t1->klucz < t2->klucz)
		return -1;
	else if (t1->klucz == t2->klucz)
		return 0;
	else
		return 1;
}

template <class T>
struct Node {

public:
	Node<T>* parent{ nullptr };
	Node<T>* leftChild{ nullptr };
	Node<T>* rightChild{ nullptr };
	T data;
	int id;
	static int UID;

	Node() : id(UID++) {}
	Node(T someData) {
		data = someData;
		id = UID++;
	}
	~Node() {}
};
template <class T>
int Node<T>::UID = 1000;

int debug = 0;

template <class T>
class BinaryTree {
private:
	Node<T>* root{ nullptr };
	int size = 0;
	int height = 0;
public:
	BinaryTree() {
	};
	BinaryTree(T data) {
		root = new Node<T>(data);
		size++;
	}
	~BinaryTree() {
		//clearTree(root);
		while (root)
			deleteNode(root);
		std::cout << "SIZE " << size << std::endl;
	}

	void addNode(T data, int (*cmp)(T t1, T t2)) {
		Node<T>* newNode = new Node<T>(data);

		if (!root) {
			root = newNode;
			size = 1;		//jak było size++ to clearTree() powodowało wejście w nullptr
		}
		else {
			Node<T>* currNode = root;
			Node<T>* parentNode = nullptr;
			while (currNode) {
				parentNode = currNode;

				if (cmp(currNode->data, data) < 0) {	//co powoduje remis kluczy?
					currNode = currNode->rightChild;
				}
				else {
					currNode = currNode->leftChild;
				}
			}
			//	currNode = newNode;
			//currNode->parent = parentNode;

				//(cmp(newNode->data, parentNode->data) < 0) ? parentNode->leftChild = newNode : parentNode->rightChild = newNode;
			if (cmp(newNode->data, parentNode->data) < 0)
				parentNode->leftChild = newNode;
			else
				parentNode->rightChild = newNode;
			newNode->parent = parentNode;
			size++;
		}
	} //A
	Node<T>* findNode(T data, int (*cmp)(T t1, T t2)) {
		Node<T>* currNode = root;
		while (currNode) {

			if (cmp(currNode->data, data) == 0) {
				//	 std::cout << "Found!" << std::endl;
				return currNode;
			}
			else if (cmp(currNode->data, data) < 0) {
				//std::cout << "Obecny node zawiera: " << currNode->data << std::endl;
		   //	 std::cout << ">>>" << std::endl;
				currNode = currNode->rightChild;

			}
			else if (cmp(currNode->data, data) > 0) {
				//std::cout << "Obecny node zawiera: " << currNode->data << std::endl;
		   //	 std::cout << "<<<" << std::endl;
				currNode = currNode->leftChild;
			}

		}
		return nullptr;

	} //B
	void deleteRoot() { //czyli deleteNode bez rodziców 
		if (!(root->leftChild) && !(root->rightChild)) {
			delete root;
			root = nullptr;
			size--;
		}
		else if (!(root->leftChild) || !(root->rightChild)) {
			Node<T>* temp = nullptr;
			(root->leftChild) ? temp = root->leftChild : temp = root->rightChild;
			temp->parent = nullptr;
			delete root;
			root = temp;
			size--;
		}
		else {
			Node<T>* candidate = root->rightChild;
			while (candidate->leftChild) {
				candidate = candidate->leftChild;
			}
			//przypadek 3.1: kandydat jest prawym dzieckiem (nie trzeba robić nic!)

			//przypadek 3.2: kandydat jest lewym dzieckiem (może ale nie musi mieć prawego dziecka)
			if (candidate != root->rightChild) {


				if (candidate->rightChild) {
					candidate->rightChild->parent = candidate->parent;   //2 połączenia na podłączenie prawego dziecka
					candidate->parent->leftChild = candidate->rightChild;
				}
				else { //lewy potomek bez dziecka
					candidate->parent->leftChild = nullptr;				//nie ma prawego dziecka lewego kandydata, więc robimy null dla rodzica
				}

				candidate->rightChild = root->rightChild;
				root->rightChild->parent = candidate;
			}

			//(node == node->parent->leftChild) ? node->parent->leftChild = candidate : node->parent->rightChild = candidate;  //rodzic jest zawsze oprócz przypadku roota
			candidate->parent = nullptr;
			candidate->leftChild = root->leftChild; //lewe dziecko jest zawsze, patrz przypadek 3
			candidate->leftChild->parent = candidate;
			delete root; //połączenia zrobione, można usuwać
			root = candidate;
			size--;
		}

	}
	void deleteNode(Node<T>* node) { //tylko wskaźniki!
	//przypadek 0: root, to samo co niżej ale bez rodzica (bo root nie ma rodzica)
		if (node == root) {
			deleteRoot();
			return;
		}

		//przypadek 1: liść
		if (!(node->leftChild) && !(node->rightChild)) {
			(node == node->parent->leftChild) ? node->parent->leftChild = nullptr : node->parent->rightChild = nullptr;
			node->parent = nullptr;
			delete node;
			size--;
		}
		//przypadek 2: 1 dzieciak
		else if (!(node->leftChild) || !(node->rightChild)) {
			Node<T>* temp = nullptr;
			(node->leftChild) ? temp = node->leftChild : temp = node->rightChild;
			(node == node->parent->leftChild) ? node->parent->leftChild = temp : node->parent->rightChild = temp;
			temp->parent = node->parent;
			delete node;
			size--;
		}
		//przypadek 3: 2 dzieciaki
		else {
			Node<T>* candidate = node->rightChild;
			while (candidate->leftChild) {
				candidate = candidate->leftChild;
			}
			//przypadek 3.1: kandydat jest prawym dzieckiem (nie trzeba robić nic!)

			//przypadek 3.2: kandydat jest lewym dzieckiem prawego dziecka (może ale nie musi mieć prawego dziecka)
			if (candidate != node->rightChild) {


				if (candidate->rightChild) {
					candidate->rightChild->parent = candidate->parent;   //2 połączenia na podłączenie prawego dziecka
					candidate->parent->leftChild = candidate->rightChild;
				}
				else { //lewy potomek bez dziecka
					candidate->parent->leftChild = nullptr;				//nie ma prawego dziecka lewego kandydata, więc robimy null dla rodzica
				}

				candidate->rightChild = node->rightChild;
				node->rightChild->parent = candidate;
			}

			(node == node->parent->leftChild) ? node->parent->leftChild = candidate : node->parent->rightChild = candidate;  //rodzic jest zawsze oprócz przypadku roota
			candidate->parent = node->parent;
			candidate->leftChild = node->leftChild; //lewe dziecko jest zawsze, patrz przypadek 3
			candidate->leftChild->parent = candidate;
			delete node; //połączenia zrobione, można usuwać
			size--;
		}
	} //C
	void preOrderTraversal(std::vector<T>* values) {
		preOrderTraversal(values, root);
	}
	void preOrderTraversal(std::vector<T>* values, Node<T>* currNode) { //root->lewe poddrzewo po kolei -> prawe poddrzewo
		if (!currNode)
			return;
		values->push_back(currNode->data);
		preOrderTraversal(values, currNode->leftChild);
		preOrderTraversal(values, currNode->rightChild);
	} //D
	T* inOrderTraversal(std::vector<T>* values) {
		inOrderTraversal(values, root);
	} //E
	void inOrderTraversal(std::vector<T>* values, Node<T>* currNode) { // lewe poddrzewo (l liść->rodzic->p liść) -> root -> prawe poddrzewo
		if (!currNode)
			return;
		inOrderTraversal(values, currNode->leftChild);
		values->push_back(currNode->data);
		inOrderTraversal(values, currNode->rightChild);
	}
	void clearTree() {
		while (root)
			deleteNode(root);
		std::cout << "SIZE " << size << std::endl;
		//clearTree(root);
	} //F
	int treeHeight() {
		if (size < 2)
			return 0;

		return treeHeight(root) - 1;
	}
	int treeHeight(Node<T>* currNode) {
		if (currNode == nullptr)
			return 0;
		else {
			//znajdz najdłuższą gałąź
			int leftHeight = treeHeight(currNode->leftChild);
			int rightHeight = treeHeight(currNode->rightChild);

			if (leftHeight > rightHeight)
				return leftHeight + 1;
			else {
				return rightHeight + 1;
			}
		}
	} //G



	void clearTree(Node<T>* currNode) { //czyli postorder traversal w sumie (lewa strona->liście->rodzice->prawa strona->liscie->rodzice->root)
		//ma usuwać liście i iść w górę
		if (!currNode)
			return;
		if ((size == 6 && debug == 0)) {
			std::cout << "here" << std::endl;
			std::cout << toString() << std::endl;
			debug = 1;
		}
		if (currNode->leftChild)
			clearTree(currNode->leftChild);
		if (currNode->rightChild)
			clearTree(currNode->rightChild);
		//delete currNode;
		deleteNode(currNode);
		std::cout << "Zmniejszam rozmiar!" << " " << "Obecny rozmiar wynosi: " << size << std::endl;
		if (size == 6)
			std::cout << toString() << std::endl;
		if (size == 5)
			std::cout << toString() << std::endl;
		if (size == 0)
			std::cout << toString() << std::endl;
	}
	std::string printNode(Node<T>* currNode) {
		std::ostringstream nodeOutput;
		if (!currNode)
			return "";
		else {
			std::string parent = "NULL";
			std::string left = "NULL";
			std::string right = "NULL";
			if (currNode->parent)
				parent = std::to_string(currNode->parent->id);

			if (currNode->leftChild)
				left = std::to_string(currNode->leftChild->id);

			if (currNode->rightChild)
				right = std::to_string(currNode->rightChild->id);

			//działa
			nodeOutput << "(ID: " << currNode->id << ": [" << "p: " << parent << ", " << "l: " << left << ", " << "r: " << right << "], " << "data: " << currNode->data << ")\n";
			std::string arr = nodeOutput.str();
			//nodeOutput.str("");
			return arr;
		}

	}

	void preOrderTraversalString(Node<T>* currNode, std::string& temp, int& nodeCount) { //root->lewe poddrzewo po kolei -> prawe poddrzewo (WAŻNE!!!) musisz przekazać stringa przez referencję inaczej nie będzie działać
		if (!currNode)
			return;
		if (nodeCount <= 0)
			return;
		temp = temp + printNode(currNode);
		preOrderTraversalString(currNode->leftChild, temp, nodeCount = nodeCount - 1);
		preOrderTraversalString(currNode->rightChild, temp, nodeCount = nodeCount - 1);
	} //D


	std::string toString() {
		std::ostringstream output;
		height = treeHeight();
		output << "Drzewo wyszukiwania binarnego:\nObecny rozmiar: " << size << "\nObecna wysokosc: " << height << "\nDane:\n";
		std::string arr = output.str();
		//output.str("");
		std::string temp = "";
		int nodeCount = 20;
		preOrderTraversalString(root, temp, nodeCount);
		arr = arr + temp;

		return arr;
	}

};





//drzewo wskaźników na Pudełka
int main432423()
{	
	srand(time(NULL));
	const int MAX_ORDER = 7; // maksymalny rzad wielkosci dodawanych danych
	BinaryTree <Box*>* bst = new BinaryTree <Box*>(); // stworzenie
		for (int o = 1; o <= MAX_ORDER; o++)
		{
			const int n = pow(10, o); // rozmiar danych
			// dodawanie do drzewa
			auto t1 = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < n; i++)
				
			{
			//bst->addNode(new Box(rand() % 100000 + rand() % 100000, 'a' + rand() % 19, "cos"), comp);


			Box* temp = new Box(rand() % 100000 + rand() % 100000, 'b', "cos");
			bst->addNode(temp, comp);
			delete temp;
			}
			auto t2 = std::chrono::high_resolution_clock::now();
			auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
			std::cout<<bst->toString()<<std::endl;
			std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
			std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count()/n << std::endl;

			// wyszukiwanie
			const int m = pow(10, 4); // liczba prob wyszukiwania
			int hits = 0; // liczba trafien
			t1 = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < m; i++)
			{
				Box* temp = new Box(rand() % 100000 + rand() % 100000, 'b', "cos");
				auto result = bst->findNode(temp, comp);
				if (result != nullptr)
					hits++;
				delete temp;
			}
			t2 = std::chrono::high_resolution_clock::now();
			timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
			std::cout << "Czas trwania operacji wyszukiwania w mikrosekundach: " << timeTaken.count() << std::endl;
			std::cout << "Liczba trafien: " << hits << std::endl;
			std::cout << "Czas trwania jednej operacji wyszukiwania w mikrosekundach: " << timeTaken.count() / m << std::endl;
			bst->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych
		}
	delete bst;
	return 0;
}
//drzewo wskaźników na typy proste
int main5465546()
{
	srand(time(NULL));
	const int MAX_ORDER = 2; // maksymalny rzad wielkosci dodawanych danych
	BinaryTree <int*>* bst = new BinaryTree <int*>(); // stworzenie
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		const int n = pow(10, o); // rozmiar danych
		// dodawanie do drzewa
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)

		{
			int* temp = new int(rand()%10000 + rand()%10000);
			bst->addNode(temp, comp);
			delete temp;
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << bst->toString() << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl;

		// wyszukiwanie
		const int m = pow(10, 4); // liczba prob wyszukiwania
		int hits = 0; // liczba trafien
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < m; i++)
		{
			int* temp = new int(rand() % 10000 + rand() % 10000);
			auto result = bst->findNode(temp, comp);
			if (result != nullptr)
				hits++;
			delete temp;
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji wyszukiwania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Liczba trafien: " << hits << std::endl;
		std::cout << "Czas trwania jednej operacji wyszukiwania w mikrosekundach: " << timeTaken.count() / m << std::endl;
		bst->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych
	}
	delete bst;
	return 0;
}
//drzewo typów prostych
int main()
{
	srand(1);
	const int MAX_ORDER = 3; // maksymalny rzad wielkosci dodawanych danych
	BinaryTree <int>* bst = new BinaryTree <int>(); // stworzenie
	for (int o = 1; o <= MAX_ORDER; o++)
	{
		//int test = 10000;
		const int n = pow(10, o); // rozmiar danych
		// dodawanie do drzewa
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < n; i++)

		{
			bst->addNode(rand() % 1000000 + rand() % 1000000, comp);
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << bst->toString() << std::endl;
		std::cout << "Czas trwania operacji dodawania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Czas trwania jednej operacji dodawania w mikrosekundach: " << timeTaken.count() / n << std::endl;

		// wyszukiwanie
		const int m = pow(10, 4); // liczba prob wyszukiwania
		int hits = 0; // liczba trafien
		t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < m; i++)
		{
			auto result = bst->findNode(rand() % 1000000 + rand() % 1000000, comp);
			if (result != NULL)
				hits++;
		}
		t2 = std::chrono::high_resolution_clock::now();
		timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
		std::cout << "Czas trwania operacji wyszukiwania w mikrosekundach: " << timeTaken.count() << std::endl;
		std::cout << "Liczba trafien: " << hits << std::endl;
		std::cout << "Czas trwania jednej operacji wyszukiwania w mikrosekundach: " << timeTaken.count() / m << std::endl;
		std::cout << bst->toString() << std::endl;
		bst->clearTree(); // czyszczenie drzewa wraz z uwalnianiem pamieci danych
	}
	delete bst;
	return 0;
}





//main testowy
int main424243() {
	std::vector<int>* myVals = new std::vector<int>();
	BinaryTree <Box*>* bst = new BinaryTree <Box*>();
	bst->addNode(new Box(rand()% 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	bst->addNode(new Box(rand() % 200, 'a' + rand() % 19, "cos"), comp);
	std::cout << bst->toString() << std::endl;
	//auto found = b1->findNode(20, comp);
	//b1->deleteNode(found);
	//auto found1 = bst->findNode(5, comp);
	//std::cout << b1->printNode(found1) << std::endl
	//std::cout << "USUWAM 10" << std::endl;
	//b1->deleteNode(found1);
	std::cout << bst->toString() << std::endl;
	bst->clearTree();
	std::cout << bst->toString() << std::endl;
	delete myVals;
	delete bst;


	return 0;





}




int main1233(){
	std::vector<int>* myVals = new std::vector<int>();
	BinaryTree<int>* b1 = new BinaryTree<int>();
	b1->addNode(5, comp);
	b1->addNode(20, comp);
	b1->addNode(41, comp);
	b1->addNode(51, comp);
	b1->addNode(61, comp);
	b1->addNode(71, comp);
	b1->addNode(81, comp);
	b1->addNode(91, comp);
	b1->addNode(101, comp);
	std::cout << b1->toString() << std::endl;
	//auto found = b1->findNode(20, comp);
	//b1->deleteNode(found);
	auto found1 = b1->findNode(5, comp);
	//std::cout << b1->printNode(found1) << std::endl
	std::cout << "USUWAM 10" << std::endl;
	b1->deleteNode(found1);
	std::cout << b1->toString() << std::endl;
	b1->clearTree();

	b1->addNode(5, comp);
	b1->addNode(20, comp);
	b1->addNode(41, comp);
	b1->addNode(51, comp);
	b1->addNode(61, comp);
	b1->addNode(71, comp);
	b1->addNode(81, comp);
	b1->addNode(91, comp);
	b1->addNode(101, comp);
	std::cout << b1->toString() << std::endl;
	delete myVals;
	delete b1;

	return 0;





}
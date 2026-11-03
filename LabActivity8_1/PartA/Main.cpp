#include <iostream>
#include <string>
#include "BTree.h"

using namespace std;

int main()
{
	string s1("Hello World!");
	string s2("A");
	string s3("B");
	string s4("C");

	BTree<string>* TreeRoot;
	BTree<string>* TreeTarget;

	TreeRoot = new BTree<string>(s1); //The root
	
	//building the tree
	TreeTarget = TreeRoot;
	TreeTarget->attachLeft(new BTree<string>(s2));
	TreeTarget->attachRight(new BTree<string>(s3));
	TreeTarget = &(TreeTarget->left());
	TreeTarget->attachLeft(new BTree<string>(s4));

	//Reset the Target pointer back to the Root
	TreeTarget = TreeRoot;
	try
	{
		cout << "Key: " << TreeTarget->key() << endl;
		cout << "Key: " << TreeTarget->left().left().key() << endl;
		cout << endl;
	}
	catch (const std::domain_error& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}

	//Detach and print those detached nodes
	BTree<string>* ToDelete;
	ToDelete = TreeRoot->left().detachLeft();
	cout << "Delete: " << ToDelete->key() << endl;
	delete ToDelete;

	ToDelete = TreeRoot->detachLeft();
	cout << "Delete: " << ToDelete->key() << endl;
	delete ToDelete;

	ToDelete = TreeRoot->detachRight();
	cout << "Delete: " << ToDelete->key() << endl;
	delete ToDelete;

	ToDelete = TreeRoot;
	cout << "Delete: " << ToDelete->key() << endl;
	delete ToDelete;

	return 0;
}
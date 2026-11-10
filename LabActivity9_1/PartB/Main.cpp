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

	//A2Tree is the root
	BTree<string> A2Tree(s1);

	//Some other tree nodes
	BTree<string> STree1(s2);
	BTree<string> STree2(s3);
	BTree<string> STree3(s4);

	//STree1<-A2Tree
	A2Tree.attachLeft(&STree1);
	//STree1<-A2Tree->STree2
	A2Tree.attachRight(&STree2);

	//STree3<-STree1<-A2Tree->STree2
	A2Tree.left().attachLeft(&STree3);

	cout << "Key: " << A2Tree.key() << endl;
	cout << "Key: " << A2Tree.left().left().key() << endl;

	//Try out the other methods to see if they follow the correct sequence as in lecture 8's example
	A2Tree.traverseDepthFirst(PreOrderVisitor<string>());
	//A2Tree.traverseDepthFirst(PostOrderVisitor<string>());
	//A2Tree.traverseDepthFirst(InOrderVisitor<string>());

	cout << endl;

	//Detach and print those detached nodes
	cout << "Detached: " << A2Tree.left().detachLeft()->key() << endl;
	cout << "Detached: " << A2Tree.detachLeft()->key() << endl;
	cout << "Detached: " << A2Tree.detachRight()->key() << endl;

	return 0;
}
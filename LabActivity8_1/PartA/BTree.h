#pragma once
#include <stdexcept>

using namespace std;

template <class T>
class BTree
{
private:
	const T* fKey; // Node's value
	//Left and right nodes branching off
	BTree<T>* fLeft;
	BTree<T>* fRight;

	//Default private constructor
	BTree() : fKey // set the fKey = nullptr
	{
		fKey = (T*)0;
	//make left/right initially NIL for now
		fLeft = &NIL;
		fRight = &NIL;
	}

public:
	static BTree<T> NIL; // Sentinel, to represent nothing

	//Constructo, initialized fKey and sets left/right to NIL
	BTree(const T& aKey) : fKey(&aKey) //set fKey = &aKey
	{
		//make left/right initially NIL for now, will be attached later
		fLeft = &NIL;
		fRight = &NIL;
	}

	//Destructor destroys left and right if not NIL to ensure the whole branch of tree is gone
	~BTree()
	{
		if (fLeft != &NIL)
			delete fRight;

		if (fRight != &NIL)
			delete fRight;
	}

	/*Auxiliary methods*/
	//checks if this node is not NIL, used later in other methods
	bool isEmpty() const
	{
		return this == &NIL;
	}

	//Return fKey, only when this node is not empty/NIL
	const T& key() const
	{
		if (isEmpty())
			throw std::domain_error("Empty node!");
		return *fKey;
	}

	/*Access methods, left and right of the binary tree*/
	//Checks first if this is not empty, then return the left/right nodes
	BTree& left() const
	{
		if (isEmpty())
			throw std::domain_error("Empty node!");
			return *fLeft

	}
	BTree& right() const
	{
		if (isEmpty())
			throw std::domain_error("Empty node!");
		return *fRight;
	}

	/*Attachment methods*/
	//Assigns left/right to point to aBTree, attaching it
	//Check if this node exists, and checks if left/right haven't been taken
	void attachLeft(BTree<T>* aBTree)
	{
		if (isEmpty())
			throw std::domain_error("Empty BTree");

		if (fLeft != &NIL)
			throw std::domain_error("Non-empty sub tree");

			fLeft = aBTree; //Simply link the fLeft pointer to the new Node
	}

	void attachRight(BTree<T>* aBTree)
	{
		if (isEmpty())
			throw std::domain_error("Empty BTree");

		if (fRight != &NIL)
			throw std::domain_error("Non-empty sub tree");

		fRight = aBTree; //Simply link the fRight pointer to the new Node;
	}

	/*Detachment methods*/
	//Removes the left/right node, make it NIL, and return the detached node
	BTree* detachLeft()
	{
		if (isEmpty())
			throw std::domain_error("Empty BTree");

		BTree<T>& Result = *fLeft; //get the node at fLeft
		fLeft = &NIL; //set fLeft to NIL
		return &Result; //return the detached node
	}

	BTree* detachRight()
	{
		if (isEmpty())
			throw std::domain_error("Empty BTree");

		BTree<T>& Result = *fRight; //get the node at fRight
		fRight = &NIL; //set fRight to NIL
		return &Result;  //return the detached node
	}
};

//Sentinel implementation, represents an empty node
template <class T>
BTree<T> BTree<T>::NIL;
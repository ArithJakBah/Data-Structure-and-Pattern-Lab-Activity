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
			throw std::domain_error("Empty node!")
			return *fLeft

	}
	BTree& right() const
	{
		if (isEmpty())
			throw std::domain_error("Empty node!");
		return *fRight;
	}

	/*Access methods, left and right of the binary tree*/
	//Checks first if this is not empty, then return the left/right nodes
	void attach
};
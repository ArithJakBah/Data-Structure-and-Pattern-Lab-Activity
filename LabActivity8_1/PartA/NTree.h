#pragma once
#include <stdexcept>  //needed for exceptions

/* NTree Template Class Declaration. N used for indicating the Degree */
template<class T, int N>
class NTree
{
private:
	T key;					// The value stored at this node
	NTree<T, N>* nodes[N];	// Array of child nodes

	//private empty ntree constructor
	NTree() : key()  //empty NTree
	{
		for (int i = 0; i < N; i++)
			nodes[i] = &NIL;
	}

public:
	static NTree<T, N> NIL; // NIL node, acts as a sentinel for null nodes
	
	//public constructor
	NTree(const T& aKey) : key(aKey) //root with N subtrees
	{
		for (int i = 0; i < N; i++)
			nodes[i] = &NIL;
	}

	//destructor
	~NTree()
	{
		for (int i = 0; i < N; i++)
			if (nodes[i] != &NIL)
				delete nodes[i];
	}

	//NTree auxiliaries
	bool isEmpty() const
	{
		return this == &NIL;
	}

	const T& getKey() const
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!");

		return key;
	}

	//Accessing a subtree (operator [])
	NTree& operator[](int aIndex) const
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!");

		if ((aIndex >= 0) && (aIndex < N))
		{
			return *nodes[aIndex]; //return reference to subtree
		}
		else
			throw std::out_of_range("Illegal subtree index!");
	}

	//Attaching a new subtree
	void attachNTree(int aIndex, NTree<T, N>* aNTree)
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!");

		if ((aIndex >= 0) && (aIndex < N))
		{
			if (nodes[aIndex] != &NIL)
				throw std::domain_error("Non-Empty subtree present!");

			nodes[aIndex] = aNTree;
		}
		else
			throw std::out_of_range("Illegal subtree index!");
	}


	//Removing a Subtree
	NTree<T,N>* detachNTree(int aIndex)
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!");

		if ((aIndex >= 0) && (aIndex < N))
		{
			NTree<T, N>* Result = nodes[aIndex];
			nodes[aIndex] = &NIL;
			return  Result;
		}
		else
			throw std::out_of_range("Illegal subtree index!");
	}

};


//NTree sentinel (NEED TO BE OUTSIDE OF CLASS DEFINITION)
template<class T, int N>
NTree<T,N> NTree<T,N>::NIL;
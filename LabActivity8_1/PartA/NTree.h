#pragma once
#include <stdexcept>  //needed for exceptions

// Template class declaration : T is the stored data type, N is the maximum branching factor(degree)
template<class T, int N>
class NTree
{
private:
	T key;					// The payload/value stored in this tree node
	NTree<T, N>* nodes[N];	// Fixed-size array storing pointers to N child subtrees

	// Private default constructor used strictly to instantiate the sentinel NIL node
	NTree() : key()  // Value-initializes key to its default constructor
	{
		for (int i = 0; i < N; i++)
			nodes[i] = &NIL;		// Points all child slots to the static NIL sentinel
	}

public:
	static NTree<T, N> NIL;		// Static sentinel node representing an empty/null subtree
	
	// Public constructor to create a node with a given payload
	NTree(const T& aKey) : key(aKey)	// Copies aKey into the node's key variable
	{
		for (int i = 0; i < N; i++)
			nodes[i] = &NIL;		// Initially sets all N child pointers to NIL sentinel
	}

	// Destructor: recursively cleans up allocated child subtrees
	~NTree()
	{
		for (int i = 0; i < N; i++)
			if (nodes[i] != &NIL)		// Avoids deleting sentinel node
				delete nodes[i];	// Deletes non-NIL child, triggering recursive deletion down the branch
	}

	// Auxiliary method to check if the current node instance is the NIL sentinel
	bool isEmpty() const
	{
		return this == &NIL;	// Returns true if this node's address matches the NIL sentinel
	}

	// Getter method to retrieve the key stored at this node
	const T& getKey() const
	{
		if (isEmpty())		// Throws exception if called on the NIL sentinel
			throw std::domain_error("Empty NTree!");

		return key;		// Returns reference to node's value payload
	}

	// Overloaded subscript operator [] to access a child subtree at aIndex
	NTree& operator[](int aIndex) const
	{
		if (isEmpty())		// Cannot access subtrees of an empty NIL node
			throw std::domain_error("Empty NTree!");

		if ((aIndex >= 0) && (aIndex < N))	// Validates index is within range [0, N-1]
		{
			return *nodes[aIndex];	// Returns reference to child subtree at aIndex
		}
		else          
			throw std::out_of_range("Illegal subtree index!");  // Index out of range
	}

	// Attaches a new dynamically allocated subtree at slot aIndex
	void attachNTree(int aIndex, NTree<T, N>* aNTree)
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!");	// Cannot attach to an empty NIL node

		if ((aIndex >= 0) && (aIndex < N))	// Validates slot index
		{
			if (nodes[aIndex] != &NIL)		// Prevents overwriting an existing child subtree
				throw std::domain_error("Non-Empty subtree present!");

			nodes[aIndex] = aNTree;		// Assigns new subtree pointer to the array slot
		}
		else
			throw std::out_of_range("Illegal subtree index!");
	}


	// Removes and returns the subtree pointer at slot aIndex without deleting it
	NTree<T,N>* detachNTree(int aIndex)
	{
		if (isEmpty())
			throw std::domain_error("Empty NTree!"); // Cannot detach from an empty NIL node

		if ((aIndex >= 0) && (aIndex < N))	// Validates slot index
		{
			NTree<T, N>* Result = nodes[aIndex];	// Stores pointer to detached subtree
			nodes[aIndex] = &NIL;					// Resets slot back to NIL sentinel
			return  Result;							// Returns pointer to detached subtree
		}
		else
			throw std::out_of_range("Illegal subtree index!");
	}

	// Recursively calculates total number of valid nodes in the tree/subtree
	int getSize() const
	{
		if (isEmpty())	// Base Case: NIL node contributes 0 to size
		{
			return 0;
		}

		int count = 1;	// Count current node

		for (int i = 0; i < N; i++)	// Sum up size of all N child subtrees
		{
			count += nodes[i]->getSize();	// Recursive call on each child
		}

		return count;	// Returns total node count
	}

	// Performs depth-first search for a key, returning node pointer or nullptr
	NTree<T, N>* search(const T& aKey) const
	{
		// Base Case: Cannot search inside an empty/NIL node
		if (isEmpty())
		{
			return nullptr;	// Base Case: Cannot search inside an empty/NIL node
		}

		if (key == aKey)	// Check if current node holds matching key
		{
			return const_cast<NTree<T, N>*>(this);	// Casts away constness to return non-const pointer
		}

		for (int i = 0; i < N; i++)		// Search recursively through each child subtree
		{
			NTree<T, N>* result = nodes[i]->search(aKey);
			if (result != nullptr)	// Key found in child branch
			{
				return result; // Bubble up found node pointer immediately
			}
		}

		return nullptr; // Key not found anywhere in this branch
	}
};


// Definition of the static sentinel NIL instance outside the class template declaration
template<class T, int N>
NTree<T,N> NTree<T,N>::NIL;
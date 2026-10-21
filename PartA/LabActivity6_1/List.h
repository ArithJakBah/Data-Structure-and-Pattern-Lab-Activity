#pragma once

#include "DoublyLinkedNode.h"
#include "DoublyLinkedNodeIterator.h"

using namespace std;

template<class T>
class List
{
private:
	typedef DoublyLinkedNode<T> Node;
	Node* head; // keeping the first node
	Node* last; //keeping of the last node
	int count;

	Node* nodeAt(int index) const
	{
		if (index < 0 || index >= count)
			return &(Node::NIL);

		Node* cur;
		if (index < count / 2)
		{
			cur = head;
			for(int i = 0; i < index; ++i) cur = cur->getNext();
		}
		else
		{
			cur = last;
			for (int i = count - 1; i > index; --i) cur = cur->getPrevious();
		}
		return cur;
	}

public:
	typedef DoublyLinkedNodeIterator<T> Iterator;

	List() : head(&(Node::NIL)), last(&(Node::NIL)), count(0) {}
	List(const List&) = delete;
	List& operator=(const List&) = delete;

	~List()
	{
		Node* cur = head;
		while (cur != &(Node::NIL))
		{
			Node* next = cur->getNext();   // save before deleting
			delete cur;
			cur = next;
		}
	}

	bool isEmpty() const { return count == 0;  }
	int size() const { return count; }

	//Tail part
	void push(const T& value)
	{
		Node* newNode = new Node(value);
		if (last != &(Node::NIL))
		{
			last->append(newNode);
		}
		else
		{
			head = newNode;
		}
		last = newNode;
		++count;
	}

	T pop()
	{
		if (isEmpty()) return T();         // nothing to remove

		Node* toDelete = last;
		T value = toDelete->getValue();
		last = last->getPrevious();        // NIL if it was the only node
		if (last == &(Node::NIL))
		{
			head = &(Node::NIL);
		}
		toDelete->remove();
		--count;

		return value;
	}

	//Head part
	void pushFront(const T& value)
	{
		Node* newNode = new Node(value);
		if (head != &(Node::NIL))
		{
			head->prepend(newNode);
		}
		else
		{
			last = newNode; // list was empty
		}
		head = newNode;
		++count;
	}

	T popFront()
	{
		if (isEmpty()) return T();

		Node* toDelete = head;
		T value = toDelete->getValue();
		head = head->getNext();            // NIL if it was the only node
		if (head == &(Node::NIL))
		{
			last = &(Node::NIL);
		}
		toDelete->remove();
		--count;

		return value;
	}

	//Middle part
	// Inserts so the new element ends up AT position index
	void insert(int index, const T& value)
	{
		if (index < 0 || index > count) return;   // invalid, do nothing

		if (index == 0) 
		{ 
			pushFront(value); 
			return; 
		}
		if (index == count) 
		{ 
			push(value); 
			return; 
		}

		nodeAt(index)->prepend(new Node(value));
		++count;
	}

	T removeAt(int index)
	{
		if (index < 0 || index >= count)
		{
			return T();
		}
		if (index == 0)
		{
			return popFront();
		}
		if (index == count - 1)
		{
			return pop();
		}

		Node* target = nodeAt(index);
		T value = target->getValue();
		target->remove();
		--count;
		return value;
	}

	//[] operator
	// Bad index returns NIL's value, which is T()
	T operator[](int index) const
	{
		return nodeAt(index)->getValue();
	}

	Iterator getIterator() const
	{
		return Iterator(last);
	}
};
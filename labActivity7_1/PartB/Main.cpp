#include <iostream>
#include "DoublyLinkedNode.h"
#include "DoublyLinkedNodeIterator.h"
#include "List.h"

using namespace std;

int main()
{
	List<int> TheList;

	cout << "Testing the Improved List - From the back" << endl;

	TheList.pushBack(1);
	TheList.pushBack(2);
	TheList.pushBack(3);

	while (!TheList.isEmpty())
	{
		cout << "Popped from the Back: " << TheList.popBack() << endl;
	}

	cout << "Testing the Improved List - From the Front" << endl;

	TheList.pushFront(4);
	TheList.pushFront(5);
	TheList.pushFront(6);

	while (!TheList.isEmpty())
	{
		cout << "Popped from the Back: " << TheList.popFront() << endl;
	}

	//====== PART B STACK & QUEUE ======

	cout << "Executing Part C - List as a Stack" << endl;

	List<int> stack;

	//ppush element into stack
	stack.pushBack(1);
	stack.pushBack(2);
	stack.pushBack(3);
	stack.pushBack(4);
	stack.pushBack(5);

	// Pop elements from stack and display them (Note: Remember LIFO)
	while (!stack.isEmpty())
	{
		cout << "Popped from stack: " << stack.popBack() << endl;
	}

	cout << "Executing Part D - List as a Queue" << endl;

	List<int> queue;

	// Enqueue elements
	queue.pushBack(1);
	queue.pushBack(2);
	queue.pushBack(3);
	queue.pushBack(4);
	queue.pushBack(5);

	// Dequeue elements and display them (note: Remember FIFO)
	while (!queue.isEmpty())
	{
		cout << "Dequeued: " << queue.popFront() << endl;
	}

	return 0;
}
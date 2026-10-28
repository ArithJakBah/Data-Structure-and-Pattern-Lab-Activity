#pragma once
#include "List.h"

template<class T>
class Stack
{
private:
    List<T> container;   // the List does the real work of storing the elements
    int maxSize;         // the most elements the stack is allowed to hold

public:
    // Constructor: sets the size limit
    // The List inside starts out empty by itself.
    Stack(int aMaxSize = 10) : maxSize(aMaxSize) {}

    // Returns true if the stack has no elements.
    // Just asks the List, since it already tracks its own count.
    bool isEmpty() const { return container.isEmpty(); }

    // Returns true if the stack has reached its limit.
    // The List has no limit of its own, so the Stack compares size against maxSize.
    bool isFull() const { return container.size() >= maxSize; }

    // Returns how many elements are currently in the stack.
    int size() const { return container.size(); }

    // Adds a value to the top of the stack.
    // If the stack is already full, nothing is added and it returns false.
    // Otherwise it adds the value at the back of the List and returns true.
    bool push(const T& value)
    {
        if (isFull()) return false;
        container.pushBack(value);
        return true;
    }

    // Removes the top element and returns it.
    // The top is the back of the List, the same end that push adds to,
    // which is what makes it LIFO (last in, first out).
    // If the stack is empty, it returns T() (0 for int), so check isEmpty() first.
    T pop()
    {
        return container.popBack();
    }
};
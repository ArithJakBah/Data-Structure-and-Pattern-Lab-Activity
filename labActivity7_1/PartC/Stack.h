#pragma once
#include "List.h"

template<class T>
class Stack
{
private:
    List<T> container;   // the List does the real work
    int maxSize;

public:
    Stack(int aMaxSize = 10) : maxSize(aMaxSize) {}

    bool isEmpty() const { return container.isEmpty(); }
    bool isFull() const { return container.size() >= maxSize; }
    int size() const { return container.size(); }

    // Returns false if the stack is full
    bool push(const T& value)
    {
        if (isFull()) return false;
        container.pushBack(value);
        return true;
    }

    // LIFO: remove from the same end we add to
    T pop()
    {
        return container.popBack();   // returns T() if empty
    }
};
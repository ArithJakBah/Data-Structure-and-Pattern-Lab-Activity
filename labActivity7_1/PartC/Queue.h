#pragma once
#include "List.h"

template<class T>
class Queue
{
private:
    List<T> container;
    int maxSize;

public:
    Queue(int aMaxSize = 10) : maxSize(aMaxSize) {}

    bool isEmpty() const { return container.isEmpty(); }
    bool isFull() const { return container.size() >= maxSize; }
    int size() const { return container.size(); }

    // Returns false if the queue is full
    bool enqueue(const T& value)
    {
        if (isFull()) return false;
        container.pushBack(value);
        return true;
    }

    // FIFO: remove from the opposite end
    T dequeue()
    {
        return container.popFront();  // returns T() if empty
    }
};
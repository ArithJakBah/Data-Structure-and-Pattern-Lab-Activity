#pragma once
#include "List.h"

template<class T>
class Queue
{
private:
    List<T> container;   // the List does the real work of storing the elements
    int maxSize;         // the most elements the queue is allowed to hold

public:
    // Constructor: sets the size limit
    // The List inside starts out empty by itself.
    Queue(int aMaxSize = 10) : maxSize(aMaxSize) {}

    // Returns true if the queue has no elements.
    // Just asks the List, since it already tracks its own count.
    bool isEmpty() const { return container.isEmpty(); }

    // Returns true if the queue has reached its limit.
    // The List has no limit of its own, so the Queue compares size against maxSize.
    bool isFull() const { return container.size() >= maxSize; }

    // Returns how many elements are currently in the queue.
    int size() const { return container.size(); }

    // Adds a value to the back of the queue.
    // If the queue is already full, nothing is added and it returns false.
    // Otherwise it adds the value at the back of the List and returns true.
    bool enqueue(const T& value)
    {
        if (isFull()) return false;
        container.pushBack(value);
        return true;
    }

    // Removes the front element and returns it.
    // New elements join at the back, but this removes from the front (the
    // opposite end), so the oldest element leaves first. That is what makes
    // it FIFO (first in, first out).
    // If the queue is empty, it returns T() (0 for int), so check isEmpty() first.
    T dequeue()
    {
        return container.popFront();
    }
};
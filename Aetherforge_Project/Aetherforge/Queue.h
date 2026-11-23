// Queue.h - CONCEPT 3b: Queue ADT (FIFO), circular-array version.
// Used by ElementGate: a ward demands compounds in a fixed ORDER (first in, first out).
#pragma once

template <typename T, int N>
class Queue
{
    T items[N];
    int headIndex;
    int count;
public:
    Queue() : headIndex(0), count(0) {}

    bool enqueue(const T& value)
    {
        if (count == N) return false;
        items[(headIndex + count) % N] = value;   // wrap around the array end
        ++count;
        return true;
    }

    bool dequeue(T& out)
    {
        if (count == 0) return false;
        out = items[headIndex];
        headIndex = (headIndex + 1) % N;
        --count;
        return true;
    }

    bool front(T& out) const
    {
        if (count == 0) return false;
        out = items[headIndex];
        return true;
    }

    // Read the i-th waiting element without removing it (for display only).
    T at(int i) const { return items[(headIndex + i) % N]; }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    void clear() { headIndex = 0; count = 0; }
};

// Stack.h - CONCEPT 3a: Stack ADT (LIFO), fixed-capacity array version.
// Used for the Rewind system: every move/fusion pushes a Snapshot; 'undo' pops it.
#pragma once

template <typename T, int N>
class Stack
{
    T items[N];
    int topIndex;                          // number of stored items
public:
    Stack() : topIndex(0) {}

    // When full, the OLDEST snapshot is discarded so the newest are always kept.
    void push(const T& value)
    {
        if (topIndex == N)
        {
            for (int i = 1; i < N; ++i) items[i - 1] = items[i];
            --topIndex;
        }
        items[topIndex++] = value;
    }

    bool pop(T& out)
    {
        if (topIndex == 0) return false;
        out = items[--topIndex];
        return true;
    }

    bool peek(T& out) const
    {
        if (topIndex == 0) return false;
        out = items[topIndex - 1];
        return true;
    }

    bool isEmpty() const { return topIndex == 0; }
    int size() const { return topIndex; }
    void clear() { topIndex = 0; }
};

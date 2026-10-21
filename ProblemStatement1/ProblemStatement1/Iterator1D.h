#pragma once
#include <iostream>

using namespace std;

// Iterator for a 1D string array (the "simple bag").
// It only tracks a position inside the array; it never changes the array itself

class Iterator1D
{
private:
    string* fArray; // pointer to the first element (NOT a copy of the array)
    int     fMax;  // maximum number of elements in the array
    int   fIndex;  // current position the iterator is pointing at

public:
    // Takes the array and its size. 
    // aStart lets us build an iterator at any
    // position used by first() and last() below, defaults to 0.
    Iterator1D(std::string* aArray, int aMax, int aStart = 0);

    // The 4 required operators:
    Iterator1D& operator++();   // move to next element
    Iterator1D& operator--();   // move to previous element
    string& operator*() const;  // read current element
    bool operator==(const Iterator1D& aOther) const;  // compare two positions

    // Helpers that return an iterator at the first or last valid position.
    // stop the user going out of range.
    Iterator1D first() const;
    Iterator1D last() const;
};
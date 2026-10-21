#pragma once
#include <string>

using namespace std;

// Iterator1D acts as a custom iterator for a 1D string array.
// It tracks a position (index) within the array without owning or modifying the array data itself.
class Iterator1D
{
private:
    string* fArray; // Pointer to the first element of the array (non-owning reference)
    int fMax; // Total number of elements in the array
    int fIndex; // Current position (index) of the iterator

public:
    // Constructor: initializes the array pointer, max size, and starting index (defaults to 0)
    Iterator1D(std::string* aArray, int aMax, int aStart = 0);

    // Navigation and access operators
    Iterator1D& operator++(); // Prefix increment: moves to the next element
    Iterator1D& operator--(); // Prefix decrement: moves to the previous element
    std::string& operator*() const; // Dereference operator: returns a reference to the current string element
    bool operator==(const Iterator1D& aOther) const; // Equality operator: compares whether two iterators point to the same array and index

    // Helper functions to get boundary iterators
    Iterator1D first() const; // Returns a new iterator positioned at the first valid element (index 0)
    Iterator1D last() const; // Returns a new iterator positioned at the last valid element (index fMax - 1)
};
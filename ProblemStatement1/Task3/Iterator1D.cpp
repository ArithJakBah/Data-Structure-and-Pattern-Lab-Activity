#include "Iterator1D.h"

// Constructor implementation: sets up the internal fields
Iterator1D::Iterator1D(std::string* aArray, int aMax, int aStart)
    : fArray(aArray), fMax(aMax), fIndex(aStart)
{
}

// Prefix increment (++it): advances the index by 1 and returns the updated iterator reference
Iterator1D& Iterator1D::operator++() { ++fIndex; return *this; }

// Prefix decrement (--it): decreases the index by 1 and returns the updated iterator reference
Iterator1D& Iterator1D::operator--() { --fIndex; return *this; }

// Dereference operator (*it): returns a reference to the string item at the current fIndex
std::string& Iterator1D::operator*() const { return fArray[fIndex]; }

// Equality comparison: returns true if both iterators point to the exact same array memory address and index position
bool Iterator1D::operator==(const Iterator1D& aOther) const
{
    return fArray == aOther.fArray && fIndex == aOther.fIndex;
}

// Returns a new iterator instance locked to the very beginning of the array (index 0)
Iterator1D Iterator1D::first() const { return Iterator1D(fArray, fMax, 0); }

// Returns a new iterator instance locked to the very end of the array (index fMax - 1)
Iterator1D Iterator1D::last() const { return Iterator1D(fArray, fMax, fMax - 1); }
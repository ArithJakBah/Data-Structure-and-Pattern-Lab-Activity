#include "Iterator1D.h"

using namespace std;

// Constructor: store the array pointer, size and starting position.
Iterator1D::Iterator1D(std::string* aArray, int aMax, int aStart) : fArray(aArray), fMax(aMax), fIndex(aStart){}

// ++ : move the position forward by one.
// Returns *this as a reference to itself so the iterator can be used in chains.
Iterator1D& Iterator1D::operator++() { ++fIndex; return *this; }

// -- : move the position back by one.
Iterator1D& Iterator1D::operator--() { --fIndex; return *this; }

// * : return the element at the current position.
// It returns a reference, so the item can be read or even changed in place.
std::string& Iterator1D::operator*() const { return fArray[fIndex]; }

// == : two iterators are equal only if they point into the same array and are at the same position.
bool Iterator1D::operator==(const Iterator1D& aOther) const
{
    return fArray == aOther.fArray && fIndex == aOther.fIndex;
}

// New iterators at the two ends of the valid range (index 0 and fMax - 1).
Iterator1D Iterator1D::first() const { return Iterator1D(fArray, fMax, 0); }
Iterator1D Iterator1D::last() const { return Iterator1D(fArray, fMax, fMax - 1); }
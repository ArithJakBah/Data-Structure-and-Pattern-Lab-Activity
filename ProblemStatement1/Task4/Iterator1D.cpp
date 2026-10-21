#include "Iterator1D.h"

using namespace std;

Iterator1D::Iterator1D(string* aArray, int aMax, int aStart) : fArray(aArray), fMax(aMax), fIndex(aStart){}

Iterator1D& Iterator1D::operator++() { ++fIndex; return *this; }
Iterator1D& Iterator1D::operator--() { fIndex; return *this; }
string& Iterator1D::operator*() const { return fArray[fIndex]; }

// Equal only if the other iterator is ALSO a 1D iterator pointing to the same array and index
bool Iterator1D::operator==(const IteratorBase& aOther) const
{
    // dynamic_cast safely attempts to convert the generic IteratorBase reference to an Iterator1D pointer.
    // If 'aOther' is actually an Iterator2D, 'o' will be nullptr.
    const Iterator1D* o = dynamic_cast<const Iterator1D*>(&aOther);
    return o != nullptr && fArray == o->fArray && fIndex == o->fIndex;
}

// These return new heap-allocated instances. The Entity class will manage deleting them.
Iterator1D* Iterator1D::first() const { return new Iterator1D(fArray, fMax, 0); }
Iterator1D* Iterator1D::last() const { return new Iterator1D(fArray, fMax, fMax - 1); }
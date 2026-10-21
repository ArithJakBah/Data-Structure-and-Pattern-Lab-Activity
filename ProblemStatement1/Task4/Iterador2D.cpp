#include "Iterator2D.h"

using namespace std;

Iterator2D::Iterator2D(string(*aArray)[COLS], int aRows, int aCols, int aStart) : fArray(aArray), fRows(aRows), fCols(aCols), fIndex(aStart)
{
}

// Incrementing and decrementing just modifies the flat index, making boundary logic incredibly simple
Iterator2D& Iterator2D::operator++() { ++fIndex; return *this; }
Iterator2D& Iterator2D::operator--() { --fIndex; return *this; }

// Translates the flat 'fIndex' into 2D coordinates:
// Row = fIndex / fCols (integer division)
// Col = fIndex % fCols (remainder)
string& Iterator2D::operator*() const
{
    return fArray[fIndex / fCols][fIndex % fCols];
}

// Polymorphic equality check: ensures we are comparing against another 2D iterator
bool Iterator2D::operator==(const IteratorBase& aOther) const
{
    const Iterator2D* o = dynamic_cast<const Iterator2D*>(&aOther);
    return o != nullptr && fArray == o->fArray && fIndex == o->fIndex;
}

// Generates bounds based on the total area (Rows * Cols)
Iterator2D* Iterator2D::first() const { return new Iterator2D(fArray, fRows, fCols, 0); }
Iterator2D* Iterator2D::last() const { return new Iterator2D(fArray, fRows, fCols, fRows * fCols - 1); }
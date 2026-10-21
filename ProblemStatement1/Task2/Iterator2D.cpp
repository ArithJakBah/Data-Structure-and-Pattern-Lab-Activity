#include "Iterator2D.h"

using namespace std;

Iterator2D::Iterator2D(std::string(*aArray)[COLS], int aRows, int aCols, int aStart) : fArray(aArray), fRows(aRows), fCols(aCols), fIndex(aStart){}

// Moving through the grid is just moving the flat index.
Iterator2D& Iterator2D::operator++() { ++fIndex; return *this; }
Iterator2D& Iterator2D::operator--() { --fIndex; return *this; }

// Convert the flat index into row and column:
// row = index / cols   (how many full rows we have passed)
// column = index % cols   (how far along the current row we are)
string& Iterator2D::operator*() const
{
    return fArray[fIndex / fCols][fIndex % fCols];
}

// Equal only if same array and same position.
bool Iterator2D::operator==(const Iterator2D& aOther) const
{
    return fArray == aOther.fArray && fIndex == aOther.fIndex;
}

// Iterators at the first cell (0) and the last cell (rows * cols - 1).
Iterator2D Iterator2D::first() const { return Iterator2D(fArray, fRows, fCols, 0); }
Iterator2D Iterator2D::last() const { return Iterator2D(fArray, fRows, fCols, fRows * fCols - 1); }
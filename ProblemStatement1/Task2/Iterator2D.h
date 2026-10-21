// Iterator are for a 2D string array
// It walks the grid row by row using ONE flat index, and converts that index to (row, column) only when an element is read.

#pragma once
#include <string>

using namespace std;

// The column count must be known at compile time. C++ needs it in the type when a 2D array is passed to a function.
const int COLS = 4;

class Iterator2D
{
private:
    string(*fArray)[COLS]; // pointer to an array of COLS strings (one row)
    int fRows; // number of rows
    int fCols; // number of columns
    int fIndex; // flat position: 0 (rows * cols - 1)

public:
    Iterator2D(std::string(*aArray)[COLS], int aRows, int aCols, int aStart = 0);

    Iterator2D& operator++(); // next element
    Iterator2D& operator--(); // previous element
    string& operator*() const; // read current element
    bool operator==(const Iterator2D& aOther) const;  // compare positions

    Iterator2D first() const;
    Iterator2D last() const;
};
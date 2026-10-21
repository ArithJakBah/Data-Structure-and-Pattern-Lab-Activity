#pragma once
#include <string>
#include "IteratorBase.h"

using namespace std;

const int COLS = 4; // Fixed compile-time column count required for standard C++ 2D array parameter passing

class Iterator2D : public IteratorBase
{
private:
    string(*fArray)[COLS]; // Pointer to an array of COLS strings (represents the 2D array)
    int fRows;
    int fCols;
    int fIndex; // Tracks progress via a single "flat" 1D index (0 to rows*cols-1)

public:
    Iterator2D(string(*aArray)[COLS], int aRows, int aCols, int aStart = 0);

    Iterator2D& operator++() override;
    Iterator2D& operator--() override;
    string& operator*() const override;
    bool operator==(const IteratorBase& aOther) const override;

    Iterator2D* first() const override;
    Iterator2D* last() const override;
};
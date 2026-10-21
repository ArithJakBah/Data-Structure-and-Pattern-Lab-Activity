#pragma once
#include <string>
#include "IteratorBase.h"

using namespace std;

// Inherits publicly from IteratorBase to fulfill the contract
class Iterator1D : public IteratorBase
{
private:
    string* fArray;
    int fMax;
    int fIndex;

public:
    Iterator1D(string* aArray, int aMax, int aStart = 0);

    // The 'override' keyword tells the compiler to ensure these exactly match the virtual functions in the base class
    Iterator1D& operator++() override;
    Iterator1D& operator--() override;
    string& operator*() const override;
    bool operator==(const IteratorBase& aOther) const override;

    // Note the return type is a pointer now, matching the base class signature
    Iterator1D* first() const override;
    Iterator1D* last() const override;
};
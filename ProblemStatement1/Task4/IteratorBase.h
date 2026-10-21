#pragma once
#include <string>

using namespace std;

// Abstract base iterator: Entity only ever talks to this interface, ensuring Level 2 Polymorphism
class IteratorBase
{
public:
    // A virtual destructor is critical here! It ensures that when Entity deletes an IteratorBase pointer, 
    // the correct derived class destructor (Iterator1D or Iterator2D) is called to prevent memory leaks.
    virtual ~IteratorBase() {}

    // Pure virtual functions (= 0) force all derived classes to implement their own logic for these operators
    virtual IteratorBase& operator++() = 0;
    virtual IteratorBase& operator--() = 0;
    virtual string& operator*() const = 0;
    virtual bool operator==(const IteratorBase& aOther) const = 0;

    // These boundary methods now return dynamically allocated pointers to the base class.
    // The caller (Entity) is responsible for deleting the new object after using it.
    virtual IteratorBase* first() const = 0;
    virtual IteratorBase* last() const = 0;
};
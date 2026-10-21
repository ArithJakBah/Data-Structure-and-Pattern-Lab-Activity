#include <iostream>
#include "Entity.h"

using namespace std;

// Constructor: initializes entity properties and sets fBag to nullptr
Entity::Entity(int aID, const string& aName, int aHP) : fEntityID(aID), fName(aName), fHP(aHP), fBag(nullptr)
{
}

// Destructor: Entity owns its bag, so it frees the dynamically allocated memory to prevent memory leaks
Entity::~Entity() { delete fBag; }

// Getter and setter definitions
int Entity::getID() const { return fEntityID; }
string Entity::getName() const { return fName; }
int Entity::getHP() const { return fHP; }
void Entity::setName(const string& aName) { fName = aName; }
void Entity::setHP(int aHP) { fHP = aHP; }

// Grab a new bag: deletes the current bag if different, then stores the new bag pointer
void Entity::Grab(Iterator1D* aBag)
{
    if (fBag != aBag)
    {
        delete fBag;
        fBag = aBag;
    }
}

// Release the bag: detaches fBag from the entity (sets it to nullptr) and returns the pointer (no deletion)
Iterator1D* Entity::Release()
{
    Iterator1D* temp = fBag;
    fBag = nullptr;
    return temp;
}

// Move inventory pointer to the next item
void Entity::InventoryNext()
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }

    // Boundary check: use operator == to ensure we don't move past the last element
    if (*fBag == fBag->last())
    {
        cout << fName << ": already at the last item." << endl;
        return;
    }
    ++(*fBag); // Advance the iterator using overloaded operator++
}

// Move inventory pointer to the previous item
void Entity::InventoryPrev()
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }

    // Boundary check: use operator == to ensure we don't move before the first element
    if (*fBag == fBag->first())
    {
        cout << fName << ": already at the first item." << endl;
        return;
    }
    --(*fBag); // Move back the iterator using overloaded operator --
}

// Display the currently selected item in the inventory
void Entity::InventoryGet() const
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }
    cout << fName << " is holding: " << **fBag << endl;   // *fBag evaluates to the Iterator, **fBag dereferences the iterator to get the string item
}
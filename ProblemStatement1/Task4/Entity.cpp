#include <iostream>
#include "Entity.h"

using namespace std;

Entity::Entity(int aID, const string& aName, int aHP) : fEntityID(aID), fName(aName), fHP(aHP), fBag(nullptr)
{
}

// Thanks to the virtual destructor in IteratorBase, this safely deletes whatever derived type is currently held
Entity::~Entity() { delete fBag; }

int Entity::getID() const { return fEntityID; }
string Entity::getName() const { return fName; }
int Entity::getHP() const { return fHP; }
void Entity::setName(const string& aName) { fName = aName; }
void Entity::setHP(int aHP) { fHP = aHP; }

void Entity::Grab(IteratorBase* aBag)
{
    if (fBag != aBag)
    {
        delete fBag;
        fBag = aBag;
    }
}

IteratorBase* Entity::Release()
{
    IteratorBase* temp = fBag;
    fBag = nullptr;
    return temp;
}

void Entity::InventoryNext()
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }

    // Create a temporary boundary marker for the last element.
    IteratorBase* end = fBag->last();

    // Check if current position matches the boundary
    bool atEnd = (*fBag == *end);

    // Because last() returned a heap-allocated pointer (new Iterator...), we MUST delete it to prevent memory leaks
    delete end;

    if (atEnd) cout << fName << ": already at the last item." << endl;
    else ++(*fBag); // Polymorphic virtual call: executes 1D or 2D logic automatically
}

void Entity::InventoryPrev()
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }

    // Same dynamic boundary check pattern for the first element
    IteratorBase* begin = fBag->first();
    bool atStart = (*fBag == *begin);
    delete begin;

    if (atStart) cout << fName << ": already at the first item." << endl;
    else --(*fBag);
}

void Entity::InventoryGet() const
{
    if (!fBag) { cout << fName << " has no bag." << endl; return; }
    cout << fName << " is holding: " << **fBag << endl;
}
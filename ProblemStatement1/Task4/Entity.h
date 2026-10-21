#pragma once
#include <string>
#include "IteratorBase.h"

using namespace std;

class Entity
{
private:
    int fEntityID;
    string fName;
    int fHP;
    IteratorBase* fBag; // Base pointer allows this to accept BOTH 1D and 2D bags seamlessly

public:
    Entity(int aID, const string& aName, int aHP);
    ~Entity();

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    int getID() const;
    string getName() const;
    int getHP() const;
    void setName(const string& aName);
    void setHP(int aHP);

    void Grab(IteratorBase* aBag); // Takes IteratorBase now
    IteratorBase* Release(); // Returns IteratorBase now

    void InventoryNext();
    void InventoryPrev();
    void InventoryGet() const;
};
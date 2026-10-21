#pragma once
#include <string>
#include "Iterator1D.h"

using namespace std;

// Entity represents a character/person in the game world who can hold and navigate an inventory bag.
class Entity
{
private:
    int fEntityID;
    string fName;
    int fHP;
    Iterator1D* fBag;

public:
    // Constructor to initialize Entity attributes
    Entity(int aID, const string& aName, int aHP);

    // Destructor: cleans up and frees the memory allocated for fBag
    ~Entity();

    // Rule of Three / Copy Prohibition: Prevent copying entities to avoid duplicate pointers and double-deletion of fBag
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Getters and Setters for entity attributes
    int getID() const;
    string getName() const;
    int getHP() const;
    void setName(const string& aName);
    void setHP(int aHP);

    // Bag management methods
    // Takes ownership of a new bag pointer (deletes any existing bag first)
    // Relinquishes ownership of the current bag pointer without deleting it (used for swapping)
    void Grab(Iterator1D* aBag);
    Iterator1D* Release();

    // Inventory navigation and display methods using the iterator
    void InventoryNext(); // Moves the bag iterator to the next item (guarded against going past the last item)
    void InventoryPrev(); // Moves the bag iterator to the previous item (guarded against going past the first item)
    void InventoryGet() const; // Prints the name of the item currently pointed to by the bag iterator
};
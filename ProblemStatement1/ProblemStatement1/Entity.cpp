#include <iostream>
#include "Entity.h"

using namespace std;

//Constructor: the initialiser list (after the colon) sets each field at the moment the object is created.
Entity::Entity(int aID, const string& aName, int aHP) : fEntityID(aID), fName(aName), fHP(aHP){}

// Getters: return a copy of the stored value
int Entity::getID() const { return fEntityID; }
string Entity::getName() const { return fName; }
int Entity::getHP() const { return fHP; }

// Setters: overwrite the stored value to a new one
void Entity::setID(int aID) { fEntityID = aID; }
void Entity::setName(const string& aName) { fName = aName; }
void Entity::setHP(int aHP) { fHP = aHP; }
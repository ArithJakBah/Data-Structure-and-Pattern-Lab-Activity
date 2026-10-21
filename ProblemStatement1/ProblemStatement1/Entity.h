#pragma once
#include <iostream>

using namespace std;

class Entity
{
private:
	// Private: code outside the class cannot touch these directly.
	// They can only be read/written through the getters and setters below
	int fEntityID;
	string fName;
	int fHP;

public:
	// Constructor: runs automatically when an Entity is created and sets the starting values of all fields.
	Entity(int aID, const string& aName, int aHP);

	// Getters: read a field. 'const' means the function only reads and never modifies the object, so it can be used on const Entities.
	int getID() const;
	string getName() const;
	int getHP() const;

	//Setter: write a new value into a field.
	void setID(int aID);
	void setName(const string& aName);
	void setHP(int aHP);
};
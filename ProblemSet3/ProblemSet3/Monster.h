// An enemy that can be fought.

#pragma once
#include "Entity.h"

class Monster : public Entity
{
private:
	int fHealth; // hit points left
	int fAttack; // damage dealt by the player per round

public:
	Monster(const std::string& aName, const std::string& aDescription, int aHealth, int aAttack)
		noexcept: Entity(aName, aDescription), fHealth(aHealth), fAttack(aAttack){}

	int health() const noexcept { return fHealth; }
	int attack() const noexcept { return fAttack; }
	bool isAlive() const noexcept { return fHealth > 0; }

	// Reduces health, never below 0.
	void takeDamage(int aDamage) noexcept
	{
		fHealth -= aDamage;
		if (fHealth < 0) fHealth = 0;
	}

	// Tells the visitor "I am a Monster" by calling visit( Monster& ).
	void accept(Visitor& aVisitor) override { aVisitor.visit(*this); }

	// A dead monster is removed from the room.
	bool isGone() const noexcept override { return !isAlive(); }
};
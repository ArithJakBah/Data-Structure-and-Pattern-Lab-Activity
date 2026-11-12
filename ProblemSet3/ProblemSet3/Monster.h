#pragma once
#include "Entity.h"

class Monster : public Entity
{
private:
	int fHealth;
	int fAttack;

public:
	Monster(const std::string& aName, const std::string& aDescription, int aHealth, int aAttack)
		noexcept: Entity(aName, aDescription), fHealth(aHealth), fAttack(aAttack){}

	int health() const noexcept { return fHealth; }
	int attack() const noexcept { return fAttack; }
	bool isAlive() const noexcept { return fHealth > 0; }

	void takeDamage(int aDamage) noexcept
	{
		fHealth -= aDamage;
		if (fHealth < 0) fHealth = 0;
	}
};
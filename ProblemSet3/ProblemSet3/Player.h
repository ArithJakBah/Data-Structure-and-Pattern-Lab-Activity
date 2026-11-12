#pragma once
#include <vector>
#include "Entity.h"
#include "Item.h"

class Player : public Entity
{
private:
	int fHealth;
	int fMaxHealth;
	int fAttack;
	std::vector<Item> fInventory;

public:
	Player(const std::string& aName, int aHealth, int aAttack)
		noexcept : Entity(aName, "The Chosen one"), fHealth(aHealth), fMaxHealth(aHealth), fAttack(aAttack) {}

	int health() const noexcept { return fHealth; }
	int attack() const noexcept { return fAttack; }
	bool isAlive() const noexcept { return fHealth > 0; }
	const std::vector<Item>& inventory() const noexcept { return fInventory; }

	void takeDamage(int aDamage) noexcept
	{
		fHealth -= aDamage;
		if (fHealth < 0) fHealth = 0;
	}

	void heal(int aAmount) noexcept
	{
		fHealth += aAmount;
		if (fHealth > fMaxHealth) fHealth = fMaxHealth;
	}

	void addItem(const Item& aItem) { fInventory.push_back(aItem); }
};
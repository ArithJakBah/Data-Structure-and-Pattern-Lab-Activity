// The user-controlled character.

#pragma once
#include <vector>
#include "Entity.h"
#include "Item.h"

// The user-controlled character. Has health, an attack value, a score and an inventory.
class Player : public Entity
{
private:
	int fHealth;
	int fMaxHealth; // healing never goes above this
	int fAttack;
	int fScore;
	bool fWon; // set when the treasure is taken
	std::vector<Item> fInventory; // copies of the items the player has taken

public:
	Player(const std::string& aName, int aHealth, int aAttack)
		noexcept : Entity(aName, "Gintoki"), fHealth(aHealth), fMaxHealth(aHealth), fAttack(aAttack), fScore(0), fWon(false) {}

	int health() const noexcept { return fHealth; }
	int attack() const noexcept { return fAttack; }
	int score() const noexcept { return fScore; }
	bool isAlive() const noexcept { return fHealth > 0; }
	bool hasWon() const noexcept { return fWon; }
	const std::vector<Item>& inventory() const noexcept { return fInventory; }

	// Reduces health, never below 0
	void takeDamage(int aDamage) noexcept
	{
		fHealth -= aDamage;
		if (fHealth < 0) fHealth = 0;
	}

	// Restores health, never above the maximum.
	void heal(int aAmount) noexcept
	{
		fHealth += aAmount;
		if (fHealth > fMaxHealth) fHealth = fMaxHealth;
	}

	void addItem(const Item& aItem) { fInventory.push_back(aItem); }
	void addScore(int aPoints) noexcept { fScore += aPoints; }
	void setWon() noexcept { fWon = true; }
};
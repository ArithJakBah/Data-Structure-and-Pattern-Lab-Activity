#pragma once
#include <vector>
#include "Combatant.h"
#include "Item.h"

class Player : public Combatant
{
private:
	int fScore;
	bool fWon;                       // set when the treasure is taken
	std::vector<Item> fInventory;    // TEMPORARY: replace with your own singly linked list

public:
	Player(const std::string& aName, const std::string& aDescription,
		int aMaxHealth, int aAttack, int aDefence, int aMaxMana)
		noexcept : Combatant(aName, aDescription, aMaxHealth, aAttack, aDefence, aMaxMana),
		fScore(0), fWon(false) {
	}

	int score() const noexcept { return fScore; }
	bool hasWon() const noexcept { return fWon; }
	const std::vector<Item>& inventory() const noexcept { return fInventory; }

	void addItem(const Item& aItem) { fInventory.push_back(aItem); }
	void addScore(int aPoints) noexcept { fScore += aPoints; }
	void setWon() noexcept { fWon = true; }

	void takeTurn(Combatant& aTarget) override;    // body goes in Player.cpp
};
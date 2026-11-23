#pragma once
#include "Entity.h"
#include "Visitor.h"

class Item : public Entity
{
private:
	int fHealAmount;  // HP restored when taken (potion)
	int fScoreValue; // points given when taken (gold coin, treasure)
	bool fIsTreasure;  // taking the treasure wins the game
	bool fTaken;  // set once collected, so the room can remove it

public:
	Item(const std::string& aName, const std::string& aDescription, int aHealAmount = 0, int aScoreValue = 0, bool aIsTreasure = false)
		noexcept : Entity(aName, aDescription), fHealAmount(aHealAmount), fScoreValue(aScoreValue), fIsTreasure(aIsTreasure), fTaken(false) {
	}

	int healAmount() const noexcept { return fHealAmount; }
	int scoreValue() const noexcept { return fScoreValue; }
	bool isTreasure() const noexcept { return fIsTreasure; }
	void markTaken() noexcept { fTaken = true; }

	// Tells the visitor "I am an Item" by calling visit( Item& ).
	void accept(Visitor& aVisitor) override { aVisitor.visit(*this); }

	// A collected item is removed from the room.
	bool isGone() const noexcept override { return fTaken; }
};
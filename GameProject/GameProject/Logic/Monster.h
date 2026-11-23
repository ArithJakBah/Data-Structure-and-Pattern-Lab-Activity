#pragma once
#include "Combatant.h"
#include "Visitor.h"

class Monster : public Combatant
{
public:
	Monster(const std::string& aName, const std::string& aDescription,
		int aMaxHealth, int aAttack, int aDefence, int aMaxMana = 0)
		noexcept : Combatant(aName, aDescription, aMaxHealth, aAttack, aDefence, aMaxMana) {
	}

	// Tells the visitor "I am a Monster" by calling visit( Monster& ).
	void accept(Visitor& aVisitor) override { aVisitor.visit(*this); }

	// A dead monster is removed from the room.
	bool isGone() const noexcept override { return !isAlive(); }

	void takeTurn(Combatant& aTarget) override;    // body goes in Monster.cpp
};
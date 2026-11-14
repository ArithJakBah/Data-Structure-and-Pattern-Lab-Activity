#pragma once
#include "Entity.h"

class Trap : public Entity
{
private:
	int fDamage; // damage dealt when it fires
	bool fArmed; // true until the player disarms it

public:
	Trap(const std::string& aName, const std::string& aDescription, int aDamage)
		noexcept : Entity(aName, aDescription), fDamage(aDamage), fArmed(true) {}

	int damage() const noexcept { return fDamage; }
	bool isArmed() const noexcept { return fArmed; }
	void disarm() noexcept { fArmed = false; }

	// Tells the visitor "I am a Trap" by calling visit( Trap& ).
	void accept(Visitor& aVisitor) override { aVisitor.visit(*this); }

	// A disarmed trap is removed from the room.
	bool isGone() const noexcept override { return !fArmed; }
};
#pragma once
#include "Entity.h"

class Trap : public Entity
{
private:
	int fDamage;
	bool fArmed;

public:
	Trap(const std::string& aName, const std::string& aDescription, int aDamage)
		noexcept : Entity(aName, aDescription), fDamage(aDamage), fArmed(true) {}

	int damage() const noexcept { return fDamage; }
	bool isArmed() const noexcept { return fArmed; }
	void disarm() noexcept { fArmed = false; }
};
#pragma once
#include "Entity.h"

class Item : public Entity
{
private:
	int fHealAmount;

public:
	Item(const std::string& aName, const std::string& aDescription, int aHealAmount = 0) 
		noexcept: Entity(aName, aDescription), fHealAmount(aHealAmount){}

	int healAmount() const noexcept { return fHealAmount; }
};
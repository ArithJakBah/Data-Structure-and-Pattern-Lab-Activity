// Combatant.cpp
#include "Combatant.h"

Combatant::Combatant(const std::string& aName, const std::string& aDescription,
	int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept
	: Entity(aName, aDescription),
	fHealth(aMaxHealth),
	fBaseMaxHealth(aMaxHealth),
	fBaseAttack(aAttack),
	fBaseDefence(aDefence),
	fMana(aMaxMana),
	fBaseMaxMana(aMaxMana)
{}

int Combatant::health() const noexcept { return fHealth; }
int Combatant::mana() const noexcept { return fMana; }
bool Combatant::isAlive() const noexcept { return fHealth > 0; }

int Combatant::maxHealth() const noexcept { return fBaseMaxHealth; }
int Combatant::attack() const noexcept { return fBaseAttack; }
int Combatant::defence() const noexcept { return fBaseDefence; }
int Combatant::maxMana() const noexcept { return fBaseMaxMana; }

int Combatant::damageAgainst(const Combatant& aTarget) const noexcept
{
	int raw = attack() - aTarget.defence();
	if (raw < 1)
	{
		raw = 1;
	}
	return raw;
}

void Combatant::takeDamage(int aDamage) noexcept
{
	if (aDamage < 0)
	{
		return;
	}
	fHealth -= aDamage;
	if (fHealth < 0)
	{
		fHealth = 0;
	}
}

void Combatant::heal(int aAmount) noexcept
{
	if (aAmount < 0 || !isAlive())
	{
		return;
	}
	fHealth += aAmount;
	if (fHealth > maxHealth())
	{
		fHealth = maxHealth();
	}
}

bool Combatant::spendMana(int aCost) noexcept
{
	if (aCost < 0)          
	{
		return false;
	}
	if (fMana < aCost)
	{
		return false;
	}
	fMana -= aCost;
	return true;
}

void Combatant::restoreMana(int aAmount) noexcept
{
	if (aAmount < 0)
	{
		return;
	}
	fMana += aAmount;
	if (fMana > maxMana())
	{
		fMana = maxMana();
	}
}

bool Combatant::addShield(int /*aDurability*/) noexcept { return false; }
int Combatant::shieldLayers() const noexcept { return 0; }
int Combatant::shieldPoints() const noexcept { return 0; }

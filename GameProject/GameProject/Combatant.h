#pragma once
#include "Entity.h"

class Combatant : public Entity
{
protected:
	int fHealth;
	int fBaseMaxHealth;
	int fBaseAttack;
	int fBaseDefence;
	int fMana;
	int fBaseMaxMana;

public:
	Combatant(const std::string& aName, const std::string& aDescription,
		int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept;

	int  health() const noexcept;
	int  mana() const noexcept;
	bool isAlive() const noexcept;                    // health > 0

	virtual int maxHealth() const noexcept;           // virtual: Player will add equipment bonuses
	virtual int attack() const noexcept;
	virtual int defence() const noexcept;
	virtual int maxMana() const noexcept;

	int  damageAgainst(const Combatant& aTarget) const noexcept;   // max(1, attack() - aTarget.defence())
	virtual void takeDamage(int aDamage) noexcept;    // clamp at 0; Player overrides later for shields
	void heal(int aAmount) noexcept;                  // clamp at maxHealth()
	bool spendMana(int aCost) noexcept;               // false if not enough mana
	void restoreMana(int aAmount) noexcept;           // clamp at maxMana()

	virtual void takeTurn(Combatant& aTarget) = 0;    // monsters decide automatically
};
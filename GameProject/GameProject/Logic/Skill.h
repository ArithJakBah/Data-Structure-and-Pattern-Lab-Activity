#pragma once
#include <string>
#include "Combatant.h"

// A skill the player (or later a monster) can use. The public use() is fixed:
// check -> pay the mana -> apply the effect. Each skill only fills in the effect.
class Skill
{
private:
	std::string fName;
	std::string fDescription;
	int         fManaCost;

protected:
	virtual void apply(Combatant& aUser, Combatant* aTarget) = 0;    // the effect only

public:
	Skill(const std::string& aName, const std::string& aDescription, int aManaCost) noexcept;
	virtual ~Skill() = default;

	const std::string& name() const noexcept;
	const std::string& description() const noexcept;
	int manaCost() const noexcept;

	virtual bool needsTarget() const noexcept = 0;                       // true: an enemy must be picked
	virtual bool canUse(const Combatant& aUser) const noexcept;          // enough mana, plus anything skill-specific
	bool use(Combatant& aUser, Combatant* aTarget);                      // false = nothing happened, no mana spent
};

// Deals extra damage to one enemy.
class Fireball : public Skill
{
public:
	static constexpr int BONUS_DAMAGE = 3;

	Fireball() noexcept;
	bool needsTarget() const noexcept override;

protected:
	void apply(Combatant& aUser, Combatant* aTarget) override;
};

// Restores some of the user's own health.
class Heal : public Skill
{
public:
	static constexpr int AMOUNT = 8;

	Heal() noexcept;
	bool needsTarget() const noexcept override;
	bool canUse(const Combatant& aUser) const noexcept override;         // not when already at full health

protected:
	void apply(Combatant& aUser, Combatant* aTarget) override;
};

// Pushes a new layer onto the user's shield stack.
class ShieldSpell : public Skill
{
public:
	static constexpr int DURABILITY = 6;

	ShieldSpell() noexcept;
	bool needsTarget() const noexcept override;
	bool canUse(const Combatant& aUser) const noexcept override;         // not when the stack is full

protected:
	void apply(Combatant& aUser, Combatant* aTarget) override;
};

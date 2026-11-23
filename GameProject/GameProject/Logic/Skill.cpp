#include "Skill.h"
#include "ShieldStack.h"

// ---------------- Skill ----------------
Skill::Skill(const std::string& aName, const std::string& aDescription, int aManaCost) noexcept
	: fName(aName), fDescription(aDescription), fManaCost(aManaCost)
{
}

const std::string& Skill::name() const noexcept { return fName; }
const std::string& Skill::description() const noexcept { return fDescription; }
int Skill::manaCost() const noexcept { return fManaCost; }

bool Skill::canUse(const Combatant& aUser) const noexcept
{
	return aUser.mana() >= fManaCost;
}

bool Skill::use(Combatant& aUser, Combatant* aTarget)
{
	if (!canUse(aUser))
	{
		return false;
	}
	if (needsTarget() && (aTarget == nullptr || !aTarget->isAlive()))
	{
		return false;
	}
	if (!aUser.spendMana(fManaCost))
	{
		return false;
	}
	apply(aUser, aTarget);
	return true;
}

// ---------------- Fireball ----------------
Fireball::Fireball() noexcept
	: Skill("Fireball", "Burns one enemy for extra damage", 3)
{
}

bool Fireball::needsTarget() const noexcept { return true; }

void Fireball::apply(Combatant& aUser, Combatant* aTarget)
{
	int damage = aUser.attack() + BONUS_DAMAGE - aTarget->defence();
	if (damage < 1)
	{
		damage = 1;
	}
	aTarget->takeDamage(damage);
}

// ---------------- Heal ----------------
Heal::Heal() noexcept
	: Skill("Heal", "Restores 8 of your health", 4)
{
}

bool Heal::needsTarget() const noexcept { return false; }

bool Heal::canUse(const Combatant& aUser) const noexcept
{
	return Skill::canUse(aUser) && aUser.health() < aUser.maxHealth();
}

void Heal::apply(Combatant& aUser, Combatant* /*aTarget*/)
{
	aUser.heal(AMOUNT);
}

// ---------------- ShieldSpell ----------------
ShieldSpell::ShieldSpell() noexcept
	: Skill("Shield", "Adds a shield layer that soaks up 6 damage", 3)
{
}

bool ShieldSpell::needsTarget() const noexcept { return false; }

bool ShieldSpell::canUse(const Combatant& aUser) const noexcept
{
	return Skill::canUse(aUser) && aUser.shieldLayers() < ShieldStack::MAX_LAYERS;
}

void ShieldSpell::apply(Combatant& aUser, Combatant* /*aTarget*/)
{
	aUser.addShield(DURABILITY);
}

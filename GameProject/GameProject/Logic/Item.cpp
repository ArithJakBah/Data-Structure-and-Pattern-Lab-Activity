#include "Item.h"
#include "Player.h"
#include "Skill.h"

// ---------------------------------------------------------------- Item
bool Item::canUse(const Player& /*aPlayer*/) const { return false; }
bool Item::use(Player& /*aPlayer*/) { return false; }

// ---------------------------------------------------------------- Potion
Potion::Potion(Kind aKind, int aAmount)
	: Item(aKind == Kind::Health ? "Health Potion" : "Mana Potion",
		(aKind == Kind::Health ? "Restores " + std::to_string(aAmount) + " health"
		                       : "Restores " + std::to_string(aAmount) + " mana")),
	fKind(aKind), fAmount(aAmount)
{
}

bool Potion::canUse(const Player& aPlayer) const
{
	if (fKind == Kind::Health)
	{
		return aPlayer.isAlive() && aPlayer.health() < aPlayer.maxHealth();
	}
	return aPlayer.mana() < aPlayer.maxMana();
}

bool Potion::use(Player& aPlayer)
{
	if (!canUse(aPlayer))
	{
		return false;               // never waste a potion
	}
	if (fKind == Kind::Health)
	{
		aPlayer.heal(fAmount);
	}
	else
	{
		aPlayer.restoreMana(fAmount);
	}
	return true;
}

// ---------------------------------------------------------------- SkillScroll
SkillScroll::SkillScroll(const std::string& aSkillName)
	: Item("Scroll of " + aSkillName, "Teaches the " + aSkillName + " skill"), fSkillName(aSkillName)
{
}

bool SkillScroll::canUse(const Player& aPlayer) const
{
	return !aPlayer.knowsSkill(fSkillName);
}

bool SkillScroll::use(Player& aPlayer)
{
	if (!canUse(aPlayer))
	{
		return false;
	}
	Skill* skill = createSkill(fSkillName);
	if (skill == nullptr)
	{
		return false;               // unknown skill name
	}
	return aPlayer.learnSkill(skill);
}

// ---------------------------------------------------------------- KeyFragment
KeyFragment::KeyFragment()
	: Item("Key Fragment", "A piece of the key to the treasure room")
{
}

// ---------------------------------------------------------------- Equipment
Equipment::Equipment(const std::string& aName, const std::string& aDescription, Slot aSlot,
	int aAttack, int aDefence, int aHealth, int aMana)
	: Item(aName, aDescription), fSlot(aSlot),
	fAttack(aAttack), fDefence(aDefence), fHealth(aHealth), fMana(aMana)
{
}

bool Equipment::canUse(const Player& /*aPlayer*/) const
{
	return true;
}

bool Equipment::use(Player& aPlayer)
{
	return aPlayer.equip(this);
}

Weapon::Weapon(const std::string& aName, int aAttackBonus)
	: Equipment(aName, "+" + std::to_string(aAttackBonus) + " attack", Slot::Weapon, aAttackBonus, 0, 0, 0)
{
}

Armor::Armor(const std::string& aName, int aHealthBonus, int aDefenceBonus)
	: Equipment(aName, "+" + std::to_string(aHealthBonus) + " health, +" + std::to_string(aDefenceBonus) + " defence",
		Slot::Body, 0, aDefenceBonus, aHealthBonus, 0)
{
}

Charm::Charm(const std::string& aName, int aManaBonus)
	: Equipment(aName, "+" + std::to_string(aManaBonus) + " mana", Slot::Accessory, 0, 0, 0, aManaBonus)
{
}

Aegis::Aegis(const std::string& aName, int aDurability)
	: Equipment(aName, "A shield layer (" + std::to_string(aDurability) + ") at the start of every fight",
		Slot::Accessory, 0, 0, 0, 0),
	fDurability(aDurability)
{
}

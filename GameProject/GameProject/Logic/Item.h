#pragma once
#include <string>
#include "Entity.h"
#include "Visitor.h"

class Player;

// Base class of everything the player can pick up. Each kind of item decides
// for itself what "use" means (polymorphism).
class Item : public Entity
{
private:
	bool fTaken;     // set once collected, so the room can remove it

public:
	Item(const std::string& aName, const std::string& aDescription) noexcept
		: Entity(aName, aDescription), fTaken(false) {
	}

	void markTaken() noexcept { fTaken = true; }

	// Tells the visitor "I am an Item" by calling visit( Item& ).
	void accept(Visitor& aVisitor) override { aVisitor.visit(*this); }

	// A collected item is removed from the room.
	bool isGone() const noexcept override { return fTaken; }

	virtual int  iconIndex() const noexcept = 0;               // which picture in assets/items.png
	virtual bool isConsumable() const noexcept { return false; }   // used up when used
	virtual bool isPotion() const noexcept { return false; }
	virtual bool isKeyFragment() const noexcept { return false; }
	virtual bool canUse(const Player& aPlayer) const;          // would using it do anything right now?
	virtual bool use(Player& aPlayer);                         // true = it worked
};

// ---------------------------------------------------------------- consumables
class Potion : public Item
{
public:
	enum class Kind { Health, Mana };

private:
	Kind fKind;
	int  fAmount;

public:
	Potion(Kind aKind, int aAmount);

	Kind kind() const noexcept { return fKind; }
	int  amount() const noexcept { return fAmount; }

	int  iconIndex() const noexcept override { return fKind == Kind::Health ? 0 : 1; }
	bool isConsumable() const noexcept override { return true; }
	bool isPotion() const noexcept override { return true; }
	bool canUse(const Player& aPlayer) const override;         // not when already full
	bool use(Player& aPlayer) override;
};

class SkillScroll : public Item
{
private:
	std::string fSkillName;

public:
	explicit SkillScroll(const std::string& aSkillName);

	const std::string& skillName() const noexcept { return fSkillName; }

	int  iconIndex() const noexcept override { return 2; }
	bool isConsumable() const noexcept override { return true; }
	bool canUse(const Player& aPlayer) const override;         // not when the skill is already known
	bool use(Player& aPlayer) override;
};

// ---------------------------------------------------------------- key fragments
class KeyFragment : public Item
{
public:
	KeyFragment();

	int  iconIndex() const noexcept override { return 3; }
	bool isKeyFragment() const noexcept override { return true; }
};

// ---------------------------------------------------------------- equipment
class Equipment : public Item
{
public:
	enum class Slot { Weapon = 0, Body = 1, Accessory = 2 };
	static constexpr size_t SLOT_COUNT = 3;

private:
	Slot fSlot;
	int  fAttack;
	int  fDefence;
	int  fHealth;
	int  fMana;

public:
	Equipment(const std::string& aName, const std::string& aDescription, Slot aSlot,
		int aAttack, int aDefence, int aHealth, int aMana);

	Slot slot() const noexcept { return fSlot; }
	int  attackBonus() const noexcept { return fAttack; }
	int  defenceBonus() const noexcept { return fDefence; }
	int  healthBonus() const noexcept { return fHealth; }
	int  manaBonus() const noexcept { return fMana; }

	// An Aegis gives a shield layer at the start of every fight; everything else gives 0.
	virtual int shieldDurability() const noexcept { return 0; }

	bool canUse(const Player& aPlayer) const override;         // always: it can be equipped
	bool use(Player& aPlayer) override;                        // equips itself
};

class Weapon : public Equipment
{
public:
	Weapon(const std::string& aName, int aAttackBonus);
	int iconIndex() const noexcept override { return 4; }
};

class Armor : public Equipment
{
public:
	Armor(const std::string& aName, int aHealthBonus, int aDefenceBonus);
	int iconIndex() const noexcept override { return 5; }
};

class Charm : public Equipment
{
public:
	Charm(const std::string& aName, int aManaBonus);
	int iconIndex() const noexcept override { return 6; }
};

class Aegis : public Equipment
{
private:
	int fDurability;

public:
	Aegis(const std::string& aName, int aDurability);
	int iconIndex() const noexcept override { return 7; }
	int shieldDurability() const noexcept override { return fDurability; }
};

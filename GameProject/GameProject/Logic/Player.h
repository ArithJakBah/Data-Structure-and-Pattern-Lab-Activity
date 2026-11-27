#pragma once
#include <string>
#include "Combatant.h"
#include "Inventory.h"
#include "ShieldStack.h"

class Skill;

class Player : public Combatant
{
public:
	static constexpr size_t MAX_SKILLS = 6;
	static constexpr int    FRAGMENTS_NEEDED = 3;

private:
	int fScore;
	bool fWon;                       // set when the treasure is taken

	Inventory   fInventory;          // singly linked list of the items in the bag
	Equipment*  fEquipped[Equipment::SLOT_COUNT];   // weapon, body, accessory (the player owns these)

	ShieldStack fShield;             // layers soak up damage before the HP does
	Skill*      fSkills[MAX_SKILLS]; // TEMPORARY: becomes the spellbook (doubly linked list) later
	size_t      fSkillCount;

	void afterEquipmentChange(int aOldMaxHealth, int aOldMaxMana) noexcept;

public:
	Player(const std::string& aName, const std::string& aDescription,
		int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept;
	~Player() override;

	// The player owns its skills and items, so copying would delete them twice.
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	int score() const noexcept { return fScore; }
	bool hasWon() const noexcept { return fWon; }
	void addScore(int aPoints) noexcept { fScore += aPoints; }
	void setWon() noexcept { fWon = true; }

	// ---- bag and equipment ----
	Inventory&       inventory() noexcept { return fInventory; }
	const Inventory& inventory() const noexcept { return fInventory; }
	bool pickUp(Item* aItem);                       // false when the bag is full
	bool useItem(Item* aItem);                      // consumables are used up; equipment is put on
	bool equip(Equipment* aEquipment);              // must be in the bag; the old piece goes back in
	bool unequip(Equipment::Slot aSlot);            // false when nothing is worn or the bag is full
	const Equipment* equipped(Equipment::Slot aSlot) const noexcept;
	int  keyFragments() const;

	// Worn equipment changes the stats (these are virtual in Combatant for exactly this reason)
	int attack() const noexcept override;
	int defence() const noexcept override;
	int maxHealth() const noexcept override;
	int maxMana() const noexcept override;

	// ---- skills (the player takes ownership of the pointer) ----
	bool   learnSkill(Skill* aSkill);
	bool   knowsSkill(const std::string& aName) const;
	size_t skillCount() const noexcept;
	Skill* skill(size_t aIndex) const noexcept;

	// ---- shield: damage goes through the stack first ----
	void takeDamage(int aDamage) noexcept override;
	bool addShield(int aDurability) noexcept override;
	int  shieldLayers() const noexcept override;
	int  shieldPoints() const noexcept override;
	void clearShield() noexcept;
	void beginBattle();                             // an equipped Aegis puts up its shield layer

	void takeTurn(Combatant& aTarget) override;    // a basic attack (the battle screen uses the menu instead)
};

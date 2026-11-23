#pragma once
#include <vector>
#include "Combatant.h"
#include "Item.h"
#include "ShieldStack.h"

class Skill;

class Player : public Combatant
{
public:
	static constexpr size_t MAX_SKILLS = 6;

private:
	int fScore;
	bool fWon;                       // set when the treasure is taken
	std::vector<Item> fInventory;    // TEMPORARY: replace with your own singly linked list

	ShieldStack fShield;             // layers soak up damage before the HP does
	Skill*      fSkills[MAX_SKILLS]; // TEMPORARY: becomes the spellbook (doubly linked list) later
	size_t      fSkillCount;

public:
	Player(const std::string& aName, const std::string& aDescription,
		int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept;
	~Player() override;

	// The player owns its skills, so copying would delete them twice.
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	int score() const noexcept { return fScore; }
	bool hasWon() const noexcept { return fWon; }
	const std::vector<Item>& inventory() const noexcept { return fInventory; }

	void addItem(const Item& aItem) { fInventory.push_back(aItem); }
	void addScore(int aPoints) noexcept { fScore += aPoints; }
	void setWon() noexcept { fWon = true; }

	// Skills (the player takes ownership of the pointer)
	bool   learnSkill(Skill* aSkill);
	size_t skillCount() const noexcept;
	Skill* skill(size_t aIndex) const noexcept;

	// Shield: damage goes through the stack first
	void takeDamage(int aDamage) noexcept override;
	bool addShield(int aDurability) noexcept override;
	int  shieldLayers() const noexcept override;
	int  shieldPoints() const noexcept override;
	void clearShield() noexcept;

	void takeTurn(Combatant& aTarget) override;    // a basic attack (the battle screen uses the menu instead)
};

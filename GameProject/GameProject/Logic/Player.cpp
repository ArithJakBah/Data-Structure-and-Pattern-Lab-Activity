#include "Player.h"
#include "Skill.h"

Player::Player(const std::string& aName, const std::string& aDescription,
	int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept
	: Combatant(aName, aDescription, aMaxHealth, aAttack, aDefence, aMaxMana),
	fScore(0), fWon(false), fEquipped{}, fSkills{}, fSkillCount(0)
{
}

Player::~Player()
{
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		delete fEquipped[i];
	}
	for (size_t i = 0; i < fSkillCount; i++)
	{
		delete fSkills[i];
	}
	// the inventory deletes the items still in the bag
}

// ---------------------------------------------------------------- bag and equipment
bool Player::pickUp(Item* aItem)
{
	return fInventory.add(aItem);
}

bool Player::useItem(Item* aItem)
{
	if (aItem == nullptr)
	{
		return false;
	}
	bool worked = aItem->use(*this);        // equipment puts itself on, potions and scrolls apply their effect
	if (worked && aItem->isConsumable())
	{
		fInventory.remove(aItem);
		delete aItem;                       // used up
	}
	return worked;
}

void Player::afterEquipmentChange(int aOldMaxHealth, int aOldMaxMana) noexcept
{
	// New gear that raises the maximum also gives that much right now.
	int newMaxHealth = maxHealth();
	if (newMaxHealth > aOldMaxHealth)
	{
		fHealth += newMaxHealth - aOldMaxHealth;
	}
	if (fHealth > newMaxHealth)
	{
		fHealth = newMaxHealth;
	}

	int newMaxMana = maxMana();
	if (newMaxMana > aOldMaxMana)
	{
		fMana += newMaxMana - aOldMaxMana;
	}
	if (fMana > newMaxMana)
	{
		fMana = newMaxMana;
	}
}

bool Player::equip(Equipment* aEquipment)
{
	if (aEquipment == nullptr || !fInventory.remove(aEquipment))
	{
		return false;                       // only things from the bag can be put on
	}

	int oldMaxHealth = maxHealth();
	int oldMaxMana = maxMana();

	size_t slot = static_cast<size_t>(aEquipment->slot());
	Equipment* previous = fEquipped[slot];
	fEquipped[slot] = aEquipment;
	if (previous != nullptr)
	{
		fInventory.add(previous);           // always fits: we just took one item out
	}

	afterEquipmentChange(oldMaxHealth, oldMaxMana);
	return true;
}

bool Player::unequip(Equipment::Slot aSlot)
{
	size_t slot = static_cast<size_t>(aSlot);
	Equipment* worn = fEquipped[slot];
	if (worn == nullptr || fInventory.isFull())
	{
		return false;
	}

	int oldMaxHealth = maxHealth();
	int oldMaxMana = maxMana();

	fEquipped[slot] = nullptr;
	fInventory.add(worn);

	afterEquipmentChange(oldMaxHealth, oldMaxMana);
	return true;
}

const Equipment* Player::equipped(Equipment::Slot aSlot) const noexcept
{
	return fEquipped[static_cast<size_t>(aSlot)];
}

int Player::keyFragments() const
{
	return fInventory.countKeyFragments();
}

int Player::attack() const noexcept
{
	int total = Combatant::attack();
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		if (fEquipped[i] != nullptr)
		{
			total += fEquipped[i]->attackBonus();
		}
	}
	return total;
}

int Player::defence() const noexcept
{
	int total = Combatant::defence();
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		if (fEquipped[i] != nullptr)
		{
			total += fEquipped[i]->defenceBonus();
		}
	}
	return total;
}

int Player::maxHealth() const noexcept
{
	int total = Combatant::maxHealth();
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		if (fEquipped[i] != nullptr)
		{
			total += fEquipped[i]->healthBonus();
		}
	}
	return total;
}

int Player::maxMana() const noexcept
{
	int total = Combatant::maxMana();
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		if (fEquipped[i] != nullptr)
		{
			total += fEquipped[i]->manaBonus();
		}
	}
	return total;
}

// ---------------------------------------------------------------- skills
bool Player::learnSkill(Skill* aSkill)
{
	if (aSkill == nullptr || fSkillCount >= MAX_SKILLS)
	{
		delete aSkill;
		return false;
	}
	fSkills[fSkillCount++] = aSkill;
	return true;
}

bool Player::knowsSkill(const std::string& aName) const
{
	for (size_t i = 0; i < fSkillCount; i++)
	{
		if (fSkills[i]->name() == aName)
		{
			return true;
		}
	}
	return false;
}

size_t Player::skillCount() const noexcept
{
	return fSkillCount;
}

Skill* Player::skill(size_t aIndex) const noexcept
{
	return aIndex < fSkillCount ? fSkills[aIndex] : nullptr;
}

// ---------------------------------------------------------------- shield
void Player::takeDamage(int aDamage) noexcept
{
	if (aDamage < 0)
	{
		return;
	}
	int throughShield = fShield.absorb(aDamage);      // the shield stack takes what it can
	Combatant::takeDamage(throughShield);             // the rest hurts for real
}

bool Player::addShield(int aDurability) noexcept
{
	return fShield.addLayer(aDurability);
}

int Player::shieldLayers() const noexcept
{
	return fShield.layers();
}

int Player::shieldPoints() const noexcept
{
	return fShield.points();
}

void Player::clearShield() noexcept
{
	fShield.clear();
}

void Player::beginBattle()
{
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		if (fEquipped[i] != nullptr && fEquipped[i]->shieldDurability() > 0)
		{
			addShield(fEquipped[i]->shieldDurability());
		}
	}
}

void Player::takeTurn(Combatant& aTarget)
{
	if (!isAlive())
	{
		return;
	}

	int damage = damageAgainst(aTarget);
	aTarget.takeDamage(damage);
}

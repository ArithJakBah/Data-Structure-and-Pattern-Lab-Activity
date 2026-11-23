#include "Player.h"
#include "Skill.h"

Player::Player(const std::string& aName, const std::string& aDescription,
	int aMaxHealth, int aAttack, int aDefence, int aMaxMana) noexcept
	: Combatant(aName, aDescription, aMaxHealth, aAttack, aDefence, aMaxMana),
	fScore(0), fWon(false), fSkills{}, fSkillCount(0)
{
}

Player::~Player()
{
	for (size_t i = 0; i < fSkillCount; i++)
	{
		delete fSkills[i];
	}
}

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

size_t Player::skillCount() const noexcept
{
	return fSkillCount;
}

Skill* Player::skill(size_t aIndex) const noexcept
{
	return aIndex < fSkillCount ? fSkills[aIndex] : nullptr;
}

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

void Player::takeTurn(Combatant& aTarget)
{
	if (!isAlive())
	{
		return;
	}

	int damage = damageAgainst(aTarget);
	aTarget.takeDamage(damage);
}

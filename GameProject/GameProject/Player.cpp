#include "Player.h"

void Player::takeTurn(Combatant& aTarget)
{
	if (!isAlive())
	{
		return;
	}

	int damage = damageAgainst(aTarget);
	aTarget.takeDamage(damage);
}
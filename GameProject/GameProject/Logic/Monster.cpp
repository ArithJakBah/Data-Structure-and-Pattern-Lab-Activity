#include "Monster.h"

void Monster::takeTurn(Combatant& aTarget)
{
		if (!isAlive())
		{
			return;
		}

		int damage = damageAgainst(aTarget);
		aTarget.takeDamage(damage);
}
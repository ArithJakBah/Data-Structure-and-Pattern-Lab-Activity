#include "Battle.h"

Battle::Battle(Player* aPlayer, Monster* aMonsters[], size_t aCount)
	: fQueue(16), fPlayer(aPlayer), fMonsters{}, fMonsterCount(0)
{
	for (size_t i = 0; i < aCount && i < MAX_MONSTERS; i++)
	{
		fMonsters[fMonsterCount++] = aMonsters[i];
	}

	// Player goes first, then each monster
	fQueue.enqueue(fPlayer);
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		fQueue.enqueue(fMonsters[i]);
	}
}

Monster* Battle::firstAliveMonster() const
{
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		if (fMonsters[i]->isAlive())
		{
			return fMonsters[i];
		}
	}
	return nullptr;
}

int Battle::aliveMonsterCount() const
{
	int count = 0;
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		if (fMonsters[i]->isAlive())
		{
			count++;
		}
	}
	return count;
}

bool Battle::isOver() const
{
	return !fPlayer->isAlive() || aliveMonsterCount() == 0;
}

bool Battle::playerWon() const
{
	return fPlayer->isAlive() && aliveMonsterCount() == 0;
}

Combatant* Battle::step(const PlayerAction* aAction)
{
	if (isOver() || fQueue.isEmpty())
	{
		return nullptr;
	}

	Combatant* current = fQueue.dequeue();

	// Dead combatants are dropped from the queue for good
	if (!current->isAlive())
	{
		return nullptr;
	}

	if (current == fPlayer && aAction != nullptr)
	{
		performPlayerAction(*aAction);          // the player's chosen move
	}
	else
	{
		// Choose the target fresh each turn
		Combatant* target = nullptr;
		if (current == fPlayer)
		{
			target = firstAliveMonster();
		}
		else
		{
			target = fPlayer;
		}

		if (target != nullptr)
		{
			current->takeTurn(*target);
		}
	}

	// Still alive after acting? Back of the queue.
	if (current->isAlive())
	{
		fQueue.enqueue(current);
	}

	return current;
}

void Battle::performPlayerAction(const PlayerAction& aAction)
{
	Monster* target = aAction.target;
	if (target == nullptr || !target->isAlive())
	{
		target = firstAliveMonster();
	}

	if (aAction.kind == PlayerAction::Kind::UseItem)
	{
		fPlayer->useItem(aAction.item);         // a used-up potion is deleted in here
		return;
	}
	if (aAction.kind == PlayerAction::Kind::UseSkill && aAction.skill != nullptr)
	{
		aAction.skill->use(*fPlayer, target);   // skills that act on the caster ignore the target
		return;
	}
	if (target != nullptr)
	{
		fPlayer->takeTurn(*target);             // basic attack
	}
}

Combatant* Battle::peekNext()
{
	if (fQueue.isEmpty())
	{
		return nullptr;
	}

	// Take the front one and send it to the back...
	Combatant* first = fQueue.dequeue();
	fQueue.enqueue(first);

	// ...then rotate everyone else once, so the original order is restored.
	int rest = fQueue.size() - 1;
	for (int i = 0; i < rest; i++)
	{
		fQueue.enqueue(fQueue.dequeue());
	}
	return first;
}

std::string Battle::turnOrderText()
{
	std::string text;
	int count = fQueue.size();

	for (int i = 0; i < count; i++)
	{
		Combatant* c = fQueue.dequeue();
		if (i > 0)
		{
			text += " -> ";
		}
		text += c->name();
		fQueue.enqueue(c);       // one out, one back in: the order is unchanged
	}
	return text;
}

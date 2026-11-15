#include "Battle.h"

Battle::Battle(Player* aPlayer, Monster* aMonsters[], size_t aCount)
	: fQueue(16), fPlayer(aPlayer), fMonsterCount(0)
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

Combatant* Battle::step()
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

	// Still alive after acting? Back of the queue.
	if (current->isAlive())
	{
		fQueue.enqueue(current);
	}

	return current;
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
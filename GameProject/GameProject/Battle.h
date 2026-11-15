#pragma once
#include <string>
#include "Queue.h"
#include "Player.h"
#include "Monster.h"

class Battle
{
public:
	static constexpr size_t MAX_MONSTERS = 4;

private:
	Queue<Combatant*> fQueue;
	Player* fPlayer;
	Monster* fMonsters[MAX_MONSTERS];
	size_t   fMonsterCount;

	Monster* firstAliveMonster() const;
	int      aliveMonsterCount() const;

public:
	Battle(Player* aPlayer, Monster* aMonsters[], size_t aCount);

	bool        isOver() const;
	bool        playerWon() const;
	Combatant* step();                // runs ONE turn; returns who acted, or nullptr if a dead one was skipped
	std::string turnOrderText();       // e.g. "Hero -> Goblin -> Goblin"
};
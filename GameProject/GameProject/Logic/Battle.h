#pragma once
#include <string>
#include "Queue.h"
#include "Player.h"
#include "Monster.h"
#include "Skill.h"

// What the player chose to do on their turn.
struct PlayerAction
{
	enum class Kind { Attack, UseSkill, UseItem };

	Kind     kind;
	Skill*   skill;       // used when kind == UseSkill
	Monster* target;      // chosen enemy (can be nullptr for things that act on the player)
	Item*    item;        // used when kind == UseItem

	static PlayerAction attack(Monster* aTarget)
	{
		return PlayerAction{ Kind::Attack, nullptr, aTarget, nullptr };
	}
	static PlayerAction useSkill(Skill* aSkill, Monster* aTarget)
	{
		return PlayerAction{ Kind::UseSkill, aSkill, aTarget, nullptr };
	}
	static PlayerAction useItem(Item* aItem)
	{
		return PlayerAction{ Kind::UseItem, nullptr, nullptr, aItem };
	}
};

class Battle
{
public:
	static constexpr size_t MAX_MONSTERS = 4;

private:
	Queue<Combatant*> fQueue;
	Player*  fPlayer;
	Monster* fMonsters[MAX_MONSTERS];
	size_t   fMonsterCount;

	Monster* firstAliveMonster() const;
	int      aliveMonsterCount() const;
	void     performPlayerAction(const PlayerAction& aAction);

public:
	Battle(Player* aPlayer, Monster* aMonsters[], size_t aCount);

	bool        isOver() const;
	bool        playerWon() const;
	Combatant*  step(const PlayerAction* aAction = nullptr);   // runs ONE turn; returns who acted, or nullptr if a dead one was skipped
	Combatant*  peekNext();                                    // who is at the front of the queue (queue order is unchanged)
	std::string turnOrderText();                               // e.g. "Hero -> Skeleton -> Skeleton"
};

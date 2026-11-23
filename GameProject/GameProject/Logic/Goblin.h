#pragma once
#include "Combatant.h"
#include "Monster.h"

class Goblin : public Monster
{
public:
	Goblin() noexcept : Monster("Goblin", "A sneaky green thief", 10, 3, 1) {}   // HP, attack, defence
};

#pragma once
#include "Monster.h"

class Skeleton : public Monster
{
public:
	Skeleton() noexcept : Monster("Skeleton", "A pile of rattling bones", 10, 3, 1) {}   // HP, attack, defence
};

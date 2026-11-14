#include <iostream>
#include "Player.h"
#include "Monster.h"
#include "Goblin.h"

static int gFailures = 0;

static void check(const char* aLabel, bool aCondition)
{
	std::cout << (aCondition ? "[PASS] " : "[FAIL] ") << aLabel << '\n';
	if (!aCondition)
	{
		gFailures++;
	}
}

int main()
{
	std::cout << "--- health: clamping and guards ---\n";
	Player hero("Hero", "The player", 20, 5, 2, 10);
	check("hero starts at full health", hero.health() == 20);

	hero.takeDamage(-5);
	check("negative damage is ignored", hero.health() == 20);

	hero.takeDamage(7);
	check("takeDamage(7) leaves 13", hero.health() == 13);

	hero.heal(999);
	check("heal stops at max health", hero.health() == 20);

	hero.takeDamage(999);
	check("overkill damage clamps at 0", hero.health() == 0);
	check("isAlive is false at 0", !hero.isAlive());

	hero.heal(10);
	check("heal does not revive the dead", hero.health() == 0);

	std::cout << "\n--- mana ---\n";
	Player mage("Mage", "A caster", 15, 2, 1, 10);
	check("mage starts with full mana", mage.mana() == 10);
	check("spendMana(-5) is rejected", !mage.spendMana(-5) && mage.mana() == 10);
	check("spendMana(4) succeeds", mage.spendMana(4) && mage.mana() == 6);
	check("spendMana(7) fails with 6 mana", !mage.spendMana(7) && mage.mana() == 6);
	mage.restoreMana(999);
	check("restoreMana stops at max mana", mage.mana() == 10);

	std::cout << "\n--- Goblin stats ---\n";
	Goblin gob;
	check("goblin health is 10", gob.health() == 10);
	check("goblin attack is 3", gob.attack() == 3);
	check("goblin defence is 1", gob.defence() == 1);
	check("goblin mana is 0", gob.mana() == 0);

	std::cout << "\n--- damage formula ---\n";
	Player fighter("Fighter", "Test fighter", 20, 5, 2, 0);
	Monster weakling("Weakling", "Hits like a feather", 10, 3, 0);
	Player tank("Tank", "Very armoured", 20, 1, 5, 0);
	check("attack 5 vs defence 1 deals 4", fighter.damageAgainst(gob) == 4);
	check("attack 3 vs defence 2 deals 1", gob.damageAgainst(fighter) == 1);
	check("attack 3 vs defence 5 still deals 1", weakling.damageAgainst(tank) == 1);

	std::cout << "\n--- one round through Combatant pointers ---\n";
	Combatant* combatants[2] = { &fighter, &gob };
	for (int i = 0; i < 2; i++)
	{
		combatants[i]->takeTurn(*combatants[1 - i]);
		std::cout << combatants[i]->name() << " acted. "
			<< combatants[0]->name() << " HP " << combatants[0]->health() << ", "
			<< combatants[1]->name() << " HP " << combatants[1]->health() << '\n';
	}
	check("goblin took 4 (10 -> 6)", gob.health() == 6);
	check("fighter took 1 (20 -> 19)", fighter.health() == 19);

	std::cout << "\n--- fight to the end ---\n";
	int rounds = 1;
	while (fighter.isAlive() && gob.isAlive() && rounds < 50)
	{
		fighter.takeTurn(gob);
		if (gob.isAlive())
		{
			gob.takeTurn(fighter);
		}
		rounds++;
		std::cout << "Round " << rounds << ": Fighter HP " << fighter.health()
			<< ", Goblin HP " << gob.health() << '\n';
	}
	check("fight ended after 3 rounds", rounds == 3);
	check("goblin is dead", !gob.isAlive());
	check("fighter survived with 18", fighter.health() == 18);

	Entity* asEntity = &gob;
	check("isGone is true for the dead goblin (through Entity*)", asEntity->isGone());
	check("isGone is false for the living fighter", !fighter.isGone());

	std::cout << "\n--- virtual destructor ---\n";
	Combatant* heap = new Goblin();
	delete heap;
	std::cout << "deleted a Goblin through a Combatant pointer without crashing\n";

	std::cout << "\nFailures: " << gFailures << '\n';
	return gFailures == 0 ? 0 : 1;
}
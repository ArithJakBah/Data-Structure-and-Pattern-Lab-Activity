// Implementation of the attack action.

#include "AttackVisitor.h"
#include "Monster.h"
#include "Trap.h"
#include "Item.h"
#include <iostream>

using namespace std;

// One round of combat: the player hits first, then a surviving monster hits back.
void AttackVisitor::visit(Monster& aMonster)
{
    if (!aMonster.isAlive()) return;
    fActed = true;

    // Player's hit
    aMonster.takeDamage(fPlayer.attack());
    cout << "You hit " << aMonster.name() << " for " << fPlayer.attack()
        << " damage. (" << aMonster.health() << " HP left)" << endl;

    if (!aMonster.isAlive())
    {
        cout << "You defeated " << aMonster.name() << "!" << endl;
        return;
    }

    // Monster hits back (only if it survived)
    fPlayer.takeDamage(aMonster.attack());
    cout << aMonster.name() << " hits you for " << aMonster.attack()
        << " damage. (You have " << fPlayer.health() << " HP)" << endl;
}

// Nothing to attack here, so these do nothing.
void AttackVisitor::visit(Trap&) {}
void AttackVisitor::visit(Item&) {}
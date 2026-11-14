// Implementation of the take action.

#include "TakeVisitor.h"
#include "Monster.h"
#include "Trap.h"
#include "Item.h"
#include <iostream>

using namespace std;

// Picks up an item: inventory, healing, score, and the win check.
void TakeVisitor::visit(Item& aItem)
{
    if (aItem.isGone()) return;
    fActed = true;

    fPlayer.addItem(aItem);  // copy into the inventory
    cout << "You take the " << aItem.name() << "." << endl;

    if (aItem.healAmount() > 0)
    {
        fPlayer.heal(aItem.healAmount());
        cout << "You recover " << aItem.healAmount() << " HP. (You have "
            << fPlayer.health() << " HP)" << endl;
    }
    if (aItem.scoreValue() > 0)
    {
        fPlayer.addScore(aItem.scoreValue());
        cout << "+" << aItem.scoreValue() << " points. (Score: " << fPlayer.score() << ")" << endl;
    }
    // Taking the treasure wins the game
    if (aItem.isTreasure())
    {
        fPlayer.setWon();
    }

    aItem.markTaken(); // the room will remove it
}

// Monsters and traps cannot be taken, so these do nothing.
void TakeVisitor::visit(Monster&) {}
void TakeVisitor::visit(Trap&) {}
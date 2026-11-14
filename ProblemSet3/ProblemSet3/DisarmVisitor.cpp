// Implementation of the disarm action.

#include "DisarmVisitor.h"
#include "Monster.h"
#include "Trap.h"
#include "Item.h"
#include <iostream>

using namespace std;

// Disarms an armed trap. 
// isGone() then becomes true, so the room removes it.
void DisarmVisitor::visit(Trap& aTrap)
{
    if (!aTrap.isArmed()) return;
    aTrap.disarm();
    fActed = true;
    cout << "You carefully disarm the " << aTrap.name() << "." << endl;
}

// Monsters and items cannot be disarmed, so these do nothing.
void DisarmVisitor::visit(Monster&) {}
void DisarmVisitor::visit(Item&) {}
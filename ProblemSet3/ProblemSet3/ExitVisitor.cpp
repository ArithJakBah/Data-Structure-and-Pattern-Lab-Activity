// Implementation of the exit check

#include "ExitVisitor.h"
#include "Monster.h"
#include "Trap.h"
#include "Item.h"

// A living monster blocks the doors until it is defeated.
void ExitVisitor::visit(Monster& aMonster)
{
    if (aMonster.isAlive())
    {
        fBlocked = true;
        fBlockerName = aMonster.name();
    }
}

// An armed trap fires when the player walks through a door.
void ExitVisitor::visit(Trap& aTrap)
{
    if (aTrap.isArmed())
    {
        fTrapDamage += aTrap.damage();
        fTrapName = aTrap.name();
    }
}

// Items never stop the player from leaving.
void ExitVisitor::visit(Item&) {}
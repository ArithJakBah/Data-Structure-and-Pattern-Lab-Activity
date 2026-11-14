// action: take. Item will go into the inventory.
// give score and healing, and the treasure wins the game.

#pragma once
#include "Visitor.h"
#include "Player.h"

class TakeVisitor : public Visitor
{
private:
    Player& fPlayer;   // who receives the item
    bool fActed;       // true if something was taken

public:
    explicit TakeVisitor(Player& aPlayer) noexcept : fPlayer(aPlayer), fActed(false) {}

    void visit(Monster& aMonster) override; // does nothing
    void visit(Trap& aTrap) override; // does nothing
    void visit(Item& aItem) override; // takes the item

    bool acted() const noexcept { return fActed; }
};
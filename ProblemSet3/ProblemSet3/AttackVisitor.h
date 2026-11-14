// The attack action.
// Only Monsters react traps and items are ignored.

#pragma once
#include "Visitor.h"
#include "Player.h"

class AttackVisitor : public Visitor
{
private:
    Player& fPlayer; // the attacker | the player
    bool fActed; // true if a monster was actually attack

public:
    explicit AttackVisitor(Player& aPlayer) noexcept : fPlayer(aPlayer), fActed(false) {}

    void visit(Monster& aMonster) override; // fights the monster
    void visit(Trap& aTrap) override;  // does nothing
    void visit(Item& aItem) override;  // does nothing

    // Tells the game loop whether the action actually did anything.
    bool acted() const noexcept { return fActed; }

};
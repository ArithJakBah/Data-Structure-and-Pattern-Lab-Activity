// action: disarm. Only armed Traps react

#pragma once
#include "Visitor.h"

class DisarmVisitor : public Visitor
{
private:
    bool fActed; // true if a trap was actually disarmed

public:
    DisarmVisitor() noexcept : fActed(false) {}

    void visit(Monster& aMonster) override; // does nothing
    void visit(Trap& aTrap) override; // disarms the trap
    void visit(Item& aItem) override; // does nothing

    // Tells the game loop whether the action actually did anything.
    bool acted() const noexcept { return fActed; }
};
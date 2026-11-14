// rule check, run when the player tries to leave a room. 
// It only gathers information. The game loop decides what to do with it.

#pragma once
#include <string>
#include "Visitor.h"

class ExitVisitor : public Visitor
{
private:
    bool fBlocked; // a living monster is in the room
    std::string fBlockerName;
    int fTrapDamage; // total damage from armed traps
    std::string fTrapName;

public:
    ExitVisitor() noexcept : fBlocked(false), fTrapDamage(0) {}

    void visit(Monster& aMonster) override; // living monster blocks the door
    void visit(Trap& aTrap) override;  // armed trap adds damage
    void visit(Item& aItem) override; // does nothing

    bool blocked() const noexcept { return fBlocked; }
    const std::string& blockerName() const noexcept { return fBlockerName; }
    int trapDamage() const noexcept { return fTrapDamage; }
    const std::string& trapName() const noexcept { return fTrapName; }

};
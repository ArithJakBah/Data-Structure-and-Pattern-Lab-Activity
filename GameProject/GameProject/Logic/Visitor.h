#pragma once

// Forward declarations only: the entity headers include this file
class Monster;
class Trap;
class Item;

class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual void visit(Monster& aMonster) = 0;
    virtual void visit(Trap& aTrap) = 0;
    virtual void visit(Item& aItem) = 0;

};
#pragma once
#include <string>

class Entity
{
protected:
    std::string fName;
    int fHp, fMaxHp, fAttack, fDefence;

public:
    Entity(const std::string& aName, int aHp, int aAttack, int aDefence);
    virtual ~Entity() = default;

    bool isAlive() const;
    void takeDamage(int aAmount);                 // clamp HP at 0

    virtual void takeTurn(Entity& aTarget) = 0;   // each subclass decides what it does
};

class Monster : public Entity
{
public:
    virtual void takeTurn(Entity& aTarget) override;   // default behaviour: attack
    virtual int  goldDrop() const;
};

class Goblin : public Monster { /* override takeTurn: maybe steals gold */ };
class Orc : public Monster { /* override takeTurn: hits harder */ };
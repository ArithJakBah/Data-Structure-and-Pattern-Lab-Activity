// Entity.h - CONCEPT 1: OOP. Abstract base class + derived classes (Inheritance),
// each overriding interact()/describe() differently (Polymorphism).
#pragma once
#include <string>
#include "Items.h"
#include "Queue.h"

class Player;

class Entity
{
protected:
    std::string name;
public:
    explicit Entity(const std::string& n) : name(n) {}
    virtual ~Entity() {}

    // Pure virtual: every derived class MUST define its own behaviour.
    virtual std::string kind() const = 0;
    virtual std::string describe() const = 0;                 // full text when you stand in the room
    virtual std::string summary() const = 0;                  // one-liner for the Scout visitor
    virtual std::string interact(Player& p, int itemId) = 0;  // the 'use' command

    virtual bool blocksPassage() const { return false; }
    virtual int saveState() const = 0;
    virtual void loadState(int state) = 0;
};

// Gives items + Essence once.
class ResourceNode : public Entity
{
    int itemId, itemQty, essenceQty;
    bool depleted;
public:
    ResourceNode(const std::string& n, int item, int qty, int essence);
    std::string kind() const override { return "Resource Node"; }
    std::string describe() const override;
    std::string summary() const override;
    std::string interact(Player& p, int itemId) override;
    int saveState() const override { return depleted ? 1 : 0; }
    void loadState(int s) override { depleted = (s != 0); }
};

// Restores Energy once.
class Shrine : public Entity
{
    int amount;
    bool used;
public:
    Shrine(const std::string& n, int restore);
    std::string kind() const override { return "Shrine"; }
    std::string describe() const override;
    std::string summary() const override;
    std::string interact(Player& p, int itemId) override;
    int saveState() const override { return used ? 1 : 0; }
    void loadState(int s) override { used = (s != 0); }
};

// Blocks the way until fed ONE specific compound as tribute.
class Guardian : public Entity
{
    int tribute;
    bool pacified;
public:
    Guardian(const std::string& n, int tributeItem);
    std::string kind() const override { return "Guardian"; }
    std::string describe() const override;
    std::string summary() const override;
    std::string interact(Player& p, int itemId) override;
    bool blocksPassage() const override { return !pacified; }
    int saveState() const override { return pacified ? 1 : 0; }
    void loadState(int s) override { pacified = (s != 0); }
};

// Blocks the way until compounds are offered in the exact ORDER required (Queue).
class ElementGate : public Entity
{
    int sequence[4];
    int total;
    bool finalGate;
    Queue<int, 4> pending;                    // compounds still required, front = next
    void rebuild(int alreadyDone);
public:
    ElementGate(const std::string& n, const int* seq, int len, bool isFinal);
    std::string kind() const override { return "Element Gate"; }
    std::string describe() const override;
    std::string summary() const override;
    std::string interact(Player& p, int itemId) override;
    bool blocksPassage() const override { return !pending.isEmpty(); }
    bool isFinal() const { return finalGate; }
    int saveState() const override { return total - pending.size(); }
    void loadState(int s) override { rebuild(s); }
};

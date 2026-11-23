// Room.h - CONCEPT 3c: Tree ADT (binary tree of rooms) + CONCEPT 4b: Visitor design pattern.
#pragma once
#include <string>
#include <iostream>
#include "Entity.h"

enum { MAX_ROOMS = 16 };

class Room;

// VISITOR PATTERN: new operations over the whole tree (scout, save, load, search)
// are added as new visitor classes without changing Room itself.
class RoomVisitor
{
public:
    virtual ~RoomVisitor() {}
    virtual void visit(Room& room) = 0;
};

class Room
{
public:
    int id;
    std::string name;
    Room* left;
    Room* right;
    Room* parent;
    Entity* entity;                       // may be nullptr (empty room)

    Room(int i, const std::string& n, Entity* e = nullptr);
    ~Room();                              // recursively frees the whole subtree

    void attachLeft(Room* child);
    void attachRight(Room* child);
    bool isBlocked() const;               // an unresolved gate/guardian seals the exits
    int depth() const;

    // Pre-order traversal (node, left, right) - the visitor is applied to every room.
    void accept(RoomVisitor& v);
};

// Prints the dungeon as an indented tree ('*' marks the player). Costs Essence in-game.
class ScoutVisitor : public RoomVisitor
{
    Room* current;
public:
    explicit ScoutVisitor(Room* here) : current(here) {}
    void visit(Room& room) override;
};

// Writes "<roomId> <state>" lines for save files.
class SaveVisitor : public RoomVisitor
{
    std::ostream& out;
public:
    explicit SaveVisitor(std::ostream& o) : out(o) {}
    void visit(Room& room) override;
};

// Pushes loaded states back into each room's entity.
class ApplyStateVisitor : public RoomVisitor
{
    int states[MAX_ROOMS];
public:
    explicit ApplyStateVisitor(const int* s) { for (int i = 0; i < MAX_ROOMS; ++i) states[i] = s[i]; }
    void visit(Room& room) override;
};

// Finds a room by id (used to restore the player's location).
class FindRoomVisitor : public RoomVisitor
{
    int target;
public:
    Room* found;
    explicit FindRoomVisitor(int id) : target(id), found(nullptr) {}
    void visit(Room& room) override;
};

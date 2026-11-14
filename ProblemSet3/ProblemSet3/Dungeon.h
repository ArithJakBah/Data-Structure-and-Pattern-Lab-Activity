// The Dungeon builds the room tree, owns the root, and remembers which room the player is currently standing in.
#pragma once
#include <memory>
#include "Room.h"

// Manages the tree of rooms: builds it, and tracks which room the player is in.
class Dungeon
{
private:
    std::unique_ptr<Room> fRoot;  // The Dungeon become the owner of the root room. the root owns every other room
    Room* fCurrent;               // The player's current room

    // Builds the whole room tree
    void build();

public:
    // Creates the dungeon, builds the tree and puts the player at the root.
    Dungeon();

    // The room the player is currently in.
    Room& current() const noexcept;

    // Moves the player through door aDoor. Returns false and stays put if that door does not exist.
    bool enter(size_t aDoor);

};
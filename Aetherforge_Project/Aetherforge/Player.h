// Player.h - the player: two RESOURCES (Energy, Essence), inventory, journal, rewind stack.
#pragma once
#include <string>
#include "Items.h"
#include "SinglyLinkedList.h"
#include "DoublyLinkedList.h"
#include "Stack.h"

class Room;

struct InvSlot { int id; int qty; };

// Everything a Rewind must restore. Inventory is stored as plain counts per item.
struct Snapshot
{
    Room* room;
    int energy;
    int essence;
    int counts[ITEM_COUNT];
};

class Player
{
public:
    enum { MAX_ENERGY = 25, MAX_JOURNAL = 30 };

    int energy;                                   // RESOURCE 1: spent on moving, gathering, mistakes
    int essence;                                  // RESOURCE 2: spent on fusing and scouting
    Room* location;
    SinglyLinkedList<InvSlot> inventory;          // Singly Linked-List
    DoublyLinkedList<std::string> journal;        // Doubly Linked-List
    Stack<Snapshot, 5> undoStack;                 // Stack

    Player();
    void reset();

    void addItem(int id, int qty);
    bool removeItem(int id, int qty);
    int countItem(int id) const;

    void gainEnergy(int amount);
    void loseEnergy(int amount);
    void note(const std::string& text);           // append to journal (capped)

    void saveSnapshot();                          // push state on the undo stack
    bool rewind();                                // pop + restore (costs 1 Energy)
    void clearUndo();                             // committed actions cannot be undone

    void exportCounts(int out[]) const;
    void importCounts(const int in[]);
};

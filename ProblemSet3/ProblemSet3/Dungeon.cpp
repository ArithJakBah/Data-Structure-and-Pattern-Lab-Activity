// Implementation of the Dungeon header class

#include "Dungeon.h"
#include "Monster.h"
#include "Trap.h"
#include "Item.h"

using namespace std;

// Create the root room, then build the rest of the tree under it.
// The player starts in the root
Dungeon::Dungeon() : fRoot(make_unique<Room>("Lobby")), fCurrent(nullptr)
{
    build();
    fCurrent = fRoot.get();
}

// Builds the tree
// addChild() creates a door to a new room; addEntity() fills the room.
void Dungeon::build()
{
    // Left branch
    Room& g1 = fRoot->addChild("Guard Room 1");
    g1.addEntity(make_unique<Monster>("Majin", "A demon blocks the way.", 30, 7));
    g1.addEntity(make_unique<Item>("Gold Coin", "Nice, A shiny gold coin.", 0, 10));

    Room& g3 = g1.addChild("Guard Room 3");
    g3.addEntity(make_unique<Monster>("Oni", "A demon blocks the way.", 40, 9));
    g3.addEntity(make_unique<Item>("Gold Coin", "Nice, A shiny gold coin.", 0, 10));

    // Two dead ends: one winning room and one losing room.
    Room& treasureA = g3.addChild("Treasure A");
    treasureA.addEntity(make_unique<Item>("Treasure", "A chest full of treasure!", 0, 100, true));

    Room& goku = g3.addChild("Goku");
    goku.addEntity(make_unique<Monster>("Goku", "A Super Saiyan Itching for a fight.", 99999, 9999));
    
    // Right branch
    Room& trapRoom = fRoot->addChild("Arrow Trap Room");
    trapRoom.addEntity(make_unique<Trap>("Arrow Trap", "Arrows shoot from the walls.", 15));
    trapRoom.addEntity(make_unique<Item>("Potion", "A red healing potion.", 20, 0));

    Room& g2 = trapRoom.addChild("Guard Room 2");
    g2.addEntity(make_unique<Monster>("Slime", "A monster blocks the way.", 20, 5));
    g2.addEntity(make_unique<Item>("Gold Coin", "Nice, A shiny gold coin.", 0, 10));

    Room& g4 = g2.addChild("Guard Room 4");
    g4.addEntity(make_unique<Monster>("Goblin", "A monster blocks the way.", 30, 7));
    g4.addEntity(make_unique<Item>("Gold Coin", "Nice, A shiny gold coin.", 0, 10));

    // Two dead ends: one winning room and one losing room.
    Room& saitama = g4.addChild("Saitama");
    saitama.addEntity(make_unique<Monster>("Saitama", "He wins every fight in one punch.", 99999, 9999));

    Room& treasureB = g4.addChild("Treasure B");
    treasureB.addEntity(make_unique<Item>("Treasure", "A chest full of treasure!", 0, 100, true));
}

Room& Dungeon::current() const noexcept { return *fCurrent; }

// Look up the child behind the chosen door. If it exists, move there.
bool Dungeon::enter(size_t aDoor)
{
    Room* next = fCurrent->door(aDoor);
    if (next == nullptr) return false;
    fCurrent = next;
    return true;
}
#include <iostream>
#include <string>
#include "Entity.h"
#include "Iterator1D.h"
#include "Iterator2D.h"

using namespace std;

// Demo function takes a step count parameter to easily navigate through the arrays
void demo(Entity& e, int steps)
{
    e.InventoryGet();
    for (int i = 0; i < steps; i++) e.InventoryNext();
    e.InventoryGet();
    e.InventoryPrev();
    e.InventoryGet();
}

int main()
{
    string bag1D[10] = { "knife", "tent", "book", "drink", "rope", "torch", "map", "apple", "compass", "blanket" };

    string bag2D[4][COLS] = {
        { "sword",  "shield", "potion", "bread"  },
        { "helmet", "boots",  "lamp",   "scroll" },
        { "bow",    "arrows", "gloves", "cloak"  },
        { "key",    "coin",   "gem",    "ring"   }
    };

    Entity alice(1, "Alice", 100);
    Entity bob(2, "Bob", 80);

    // Initial assignment: Alice gets a 1D bag, Bob gets a 2D bag
    alice.Grab(new Iterator1D(bag1D, 10));
    bob.Grab(new Iterator2D(bag2D, 4, COLS));

    cout << "=== Before swap ===" << endl;
    demo(alice, 3);
    demo(bob, 5); // 5 steps in a 4-column 2D array smoothly crosses from row 0 into row 1

    // Swap bags polymorphically. No changes needed in Entity logic!
    // Both variables 'a' and 'b' are safely handled as generic IteratorBase pointers.
    IteratorBase* a = alice.Release();
    IteratorBase* b = bob.Release();
    alice.Grab(b); // Alice now has the 2D bag
    bob.Grab(a); // Bob now has the 1D bag

    cout << "\n=== After swap ===" << endl;
    demo(alice, 3);
    demo(bob, 3);

    cout << "\n=== Boundary test ===" << endl;
    // Alice has the 2D bag (max 16 items). Pushing it 20 steps forces the boundary check.
    for (int i = 0; i < 20; i++) alice.InventoryNext();
    alice.InventoryGet();

    return 0; // Exiting main triggers Entity destructors, securely deleting the polymorphic bags
}
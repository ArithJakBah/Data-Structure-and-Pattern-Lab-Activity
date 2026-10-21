#include <iostream>
#include <string>
#include "Entity.h"

using namespace std;

// Demonstration helper function: performs a sequence of inventory navigation and retrieval actions
void demo(Entity& e)
{
    e.InventoryGet();// Read current item
    e.InventoryNext();// Move forward 1st time
    e.InventoryNext();// Move forward 2nd time
    e.InventoryGet(); // Read current item after moving
    e.InventoryPrev();// Move backward 1st time
    e.InventoryGet();// Read current item after moving back
    e.InventoryPrev();// Move backward to the first item
    e.InventoryPrev();// Boundary test: should refuse to go before the first item
}

int main()
{
    // Define raw string arrays representing items in two different bags
    string items1[10] = { "knife", "tent", "book", "drink", "rope", "torch", "map", "apple", "compass", "blanket" };
    string items2[10] = { "pen", "laptop", "phone", "wallet", "keys", "watch", "charger", "notebook", "mug", "umbrella" };

    // Instantiate two Entity objects: Alice and Bob
    Entity alice(1, "Alice", 100);
    Entity bob(2, "Bob", 80);

    // Each entity grabs a dynamically allocated Iterator1D bag created on the heap
    alice.Grab(new Iterator1D(items1, 10));
    bob.Grab(new Iterator1D(items2, 10));

    // Demonstrate inventory navigation before swapping bags
    cout << "=== Before swap ===" << endl;
    demo(alice);
    demo(bob);

    // Swap bags between Alice and Bob:
    // 1. Release() detaches the bags without deleting them.
    // 2. Grab() passes each other's bag pointer to the respective entity.
    Iterator1D* a = alice.Release();
    Iterator1D* b = bob.Release();
    alice.Grab(b);
    bob.Grab(a);

    // Demonstrate inventory navigation after swapping bags to verify they now control each other's bags
    cout << "\n=== After swap ===" << endl;
    demo(alice);
    demo(bob);

    return 0;// Destructors automatically clean up and delete the allocated bag memory
}
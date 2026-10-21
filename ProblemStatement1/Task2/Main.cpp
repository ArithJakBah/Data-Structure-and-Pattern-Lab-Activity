// Task 2: create the two bags, create an iterator for each, move them around and print what they point at.
#include <iostream>
#include <string>
#include "Iterator1D.h"
#include "Iterator2D.h"

using namespace std;

int main()
{
    // Bag 1: a 1D array holding up to 10 items, filled manually.
    string bag1D[10] = { "knife", "tent", "book", "drink", "rope",
                         "torch", "map", "apple", "compass", "blanket" };

    // Bag 2: a 2D array of 4 x 4 items, filled manually.
    string bag2D[4][COLS] = {
        { "sword",  "shield", "potion", "bread"  },
        { "helmet", "boots",  "lamp",   "scroll" },
        { "bow",    "arrows", "gloves", "cloak"  },
        { "key",    "coin",   "gem",    "ring"   }
    };

    // Create one iterator per bag by passing the array and its maximum size.
    Iterator1D it1(bag1D, 10);
    Iterator2D it2(bag2D, 4, COLS);

    // Move the 1D iterator forward 3 times, then print the current item.
    ++it1; ++it1; ++it1;
    cout << "1D after 3 increments: " << *it1 << endl; // drink

    // Move the 2D iterator forward 5 times. It passes the end of row 0 and continues into row 1.
    for (int i = 0; i < 5; i++) ++it2;
    cout << "2D after 5 increments: " << *it2 << endl; // boots

    // Step back one with --.
    --it2;
    cout << "2D after 1 decrement:  " << *it2 << endl; // helmet

    // Test ==: it1 is at index 3, so it is NOT equal to the first position.
    cout << "it1 == first? " << (it1 == it1.first() ? "yes" : "no") << endl;

    // Walk the 2D iterator to the final cell and test == against last().
    for (int i = 0; i < 11; i++) ++it2;
    cout << "2D at last element: " << *it2 << ", == last? "
        << (it2 == it2.last() ? "yes" : "no") << endl;

    return 0;
}
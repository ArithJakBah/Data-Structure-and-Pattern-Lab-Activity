#include <iostream>
#include <string>
#include "Entity.h"
using namespace std;


// Prints one entity's details using its getters. 
// Takes a const reference and no copy is made and the entity cannot be changed.
void printEntity(const Entity& e)
{
    cout << "ID: " << e.getID() << " | Name: " << e.getName()
        << " | HP: " << e.getHP() << endl;
}

// Ask the user which entity a command to apply to. Returns a POINTER to the chosen entity which is not a copy, so changes are made through it affect the real entity.
// Returns nullptr if the input is invalid.
Entity* pickEntity(Entity list[], int count)
{
    cout << "Which entity? (1-" << count << "): ";
    string line;
    getline(cin, line);  // read the whole line the user typed

    int choice = 0;
    // stoi converts text to a number and it throws an error on non-number.
    try { choice = stoi(line); }
    catch (...) {}

    if (choice < 1 || choice > count)
    {
        cout << "Invalid choice." << endl;
        return nullptr;
    }

    // The user types 1-based numbers, but arrays start at index 0, so subtract 1. 
    // '&' gives the address of that array slot
    return &list[choice - 1];                     
}

int main()
{
    // An array of Entity objects. The initialiser list is needed because the Entity has no default constructor
    const int COUNT = 2;
    Entity entities[COUNT] = { Entity(1, "Alice", 100), Entity(2, "Bob", 80) };

    // Setter/getter test
    cout << "Initial state:" << endl;
    for (int i = 0; i < COUNT; i++) printEntity(entities[i]);

    entities[0].setName("Alicia"); // setter changes the field
    entities[1].setHP(55);
    cout << "\nAfter using setters:" << endl;
    for (int i = 0; i < COUNT; i++) printEntity(entities[i]);  //getter reads it back

    // Game loop
    // It runs forever. Each pass will ask for input, match it to a command, run that command, then repeat. 
    // Only "exit" can leave the loop
    string command;
    while (true)
    {
        cout << "\nEnter command (show / damage / rename / exit): ";
        getline(cin, command);

        if (command == "show")
        {
            for (int i = 0; i < COUNT; i++) printEntity(entities[i]);
        }
        else if (command == "damage")
        {
            Entity* target = pickEntity(entities, COUNT);
            if (target)
            {
                int hp = target->getHP() - 10;
                if (hp < 0) hp = 0;                 // HP never goes negative
                target->setHP(hp);
                cout << target->getName() << " took 10 damage." << endl;
            }
        }
        else if (command == "rename")
        {
            Entity* target = pickEntity(entities, COUNT);
            if (target)
            {
                string newName;
                cout << "New name: ";
                getline(cin, newName);
                target->setName(newName);
            }
        }
        else if (command == "exit")
        {
            cout << "Goodbye!" << endl;
            break;
        }
        else
        {
            // Output if nothing match the command
            cout << "Unknown command." << endl;
        }
    }
    return 0;
}
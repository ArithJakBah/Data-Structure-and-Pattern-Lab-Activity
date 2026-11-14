#include <iostream>
#include <sstream>
#include <string>

#include "Player.h"
#include "Dungeon.h"
#include "AttackVisitor.h"
#include "DisarmVisitor.h"
#include "TakeVisitor.h"
#include "ExitVisitor.h"

using namespace std;

// Lists the commands the player can type.
void printHelp()
{
    cout << "Commands: look, go <door number>, attack, disarm, take, status, inventory, help, quit" << endl;
}

// Shows the player's current stats.
void printStatus(const Player& aPlayer)
{
    cout << aPlayer.name() << " | HP: " << aPlayer.health()
        << " | Attack: " << aPlayer.attack()
        << " | Score: " << aPlayer.score() << endl;
}

// Lists the items the player is carrying.
void printInventory(const Player& aPlayer)
{
    if (aPlayer.inventory().empty())
    {
        cout << "Your inventory is empty." << endl;
        return;
    }
    cout << "Inventory:" << endl;
    for (const Item& item : aPlayer.inventory())
    {
        cout << "  - " << item.name() << ": " << item.description() << endl;
    }
}

// Describes a room. Only the door COUNT is shown, so the next room stays a mystery.
void look(const Room& aRoom)
{
    cout << "--- " << aRoom.name() << " ---" << endl;

    if (aRoom.entities().empty())
    {
        cout << "There is nothing here." << endl;
    }
    else
    {
        cout << "You see:" << endl;
        for (const auto& entity : aRoom.entities())
        {
            cout << "  - " << entity->name() << ": " << entity->description() << endl;
        }
    }

    if (aRoom.doorCount() == 0)
    {
        cout << "There are no more doors. This is a dead end." << endl;
    }
    else if (aRoom.doorCount() == 1)
    {
        cout << "There is 1 door. (go 1)" << endl;
    }
    else
    {
        cout << "There are " << aRoom.doorCount() << " doors. (go 1 or go " << aRoom.doorCount() << ")" << endl;
    }
}

int main()
{
    Player player("Hero", 150, 20);
    Dungeon dungeon;

    cout << "=== Dungon Gem ===" << endl;
    printHelp();
    look(dungeon.current());

    // Game loop: read a command, handle it, repeat until the player quits or dies.
    bool running = true;
    while (running && player.isAlive() && !player.hasWon())
    {
        cout << "\n> ";
        string line;
        if (!getline(cin, line))
        {
            break; // input closed
        }

        // Split the line into a command word and a number.
        istringstream input(line);
        string command;
        input >> command;

        if (command == "look")
        {
            look(dungeon.current());
        }
        else if (command == "go")
        {
            // The player types doors from 1, but the code counts from 0.
            int door = 0;
            if (!(input >> door) || door < 1 || static_cast<size_t>(door) > dungeon.current().doorCount())
            {
                cout << "There is no such door." << endl;
            }
            else
            {
                // Check the exit rules first: is a monster blocking, does a trap fire?
                ExitVisitor exitCheck;
                dungeon.current().apply(exitCheck);

                if (exitCheck.blocked())
                {
                    cout << "The " << exitCheck.blockerName() << " blocks the door! Defeat it first." << endl;
                }
                else
                {
                    // Armed traps hurt the player as they pass through
                    if (exitCheck.trapDamage() > 0)
                    {
                        player.takeDamage(exitCheck.trapDamage());
                        cout << "The " << exitCheck.trapName() << " fires as you pass! You take "
                            << exitCheck.trapDamage() << " damage. (You have " << player.health() << " HP)" << endl;
                    }
                    // Move to the chosen door (typed from 1, stored from 0)
                    if (player.isAlive())
                    {
                        dungeon.enter(static_cast<size_t>(door - 1));
                        look(dungeon.current());
                    }
                }
            }
        }
        // Each action below is a visitor applied to every entity in the current room
        else if (command == "attack")
        {
            AttackVisitor attack(player);
            dungeon.current().apply(attack);
            if (!attack.acted()) cout << "There is nothing to attack here." << endl;
        }
        else if (command == "disarm")
        {
            DisarmVisitor disarm;
            dungeon.current().apply(disarm);
            if (!disarm.acted()) cout << "There is nothing to disarm here." << endl;
        }
        else if (command == "take")
        {
            TakeVisitor take(player);
            dungeon.current().apply(take);
            if (!take.acted()) cout << "There is nothing to take here." << endl;
        }

        else if (command == "status") printStatus(player);
        else if (command == "inventory") printInventory(player);
        else if (command == "help") printHelp();
        else if (command == "quit") running = false;
        else cout << "Unknown command. Type 'help'." << endl;
    }

    if (!player.isAlive())
    {
        cout << "\nYou have died. You Suck. Final score: " << player.score() << endl;
    }
    else if (player.hasWon())
    {
        cout << "\nYou found the Dragon Piece Z! You win! Final score: " << player.score() << endl;
    }
    else
    {
        cout << "Thanks for playing!" << endl;
    }
    return 0;

}
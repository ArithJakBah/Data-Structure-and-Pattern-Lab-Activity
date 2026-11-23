// Game.h - game controller: builds the world, parses commands, saves/loads.
#pragma once
#include <string>
#include "Player.h"
#include "Room.h"
#include "HashTable.h"

class Game
{
    Room* root;
    Player player;
    HashTable<int> recipes;               // "Fire+Water" -> Steam  (Hash Table)
    bool running;
    bool won;

    void buildRecipes();
    void buildDungeon();

    void printIntro() const;
    void printHelp() const;
    void printStatus() const;
    void printRoom();
    void handle(const std::string& line);

    void cmdMove(const std::string& dir);
    void cmdUse(const std::string& itemName);
    void cmdFuse(const std::string& a, const std::string& b);
    void cmdInventory();
    void cmdJournal();
    void cmdUndo();
    void cmdScout();
    void cmdSave();
    void cmdLoad();
    void checkEnd();

public:
    Game();
    ~Game();
    void run();
};

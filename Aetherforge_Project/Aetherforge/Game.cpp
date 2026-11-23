#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

static const char* SAVE_FILE = "aetherforge_save.txt";

static std::string lower(std::string s)
{
    for (size_t i = 0; i < s.size(); ++i) s[i] = (char)std::tolower((unsigned char)s[i]);
    return s;
}

Game::Game() : root(nullptr), recipes(11), running(true), won(false)
{
    buildRecipes();
    buildDungeon();
}

Game::~Game() { delete root; }

// ---------------------------------------------------------------- world setup
void Game::buildRecipes()
{
    // HASH TABLE: insert each alchemy recipe under its order-independent key.
    recipes.insert(recipeKey(FIRE,  WATER), STEAM);
    recipes.insert(recipeKey(WATER, EARTH), MUD);
    recipes.insert(recipeKey(FIRE,  EARTH), LAVA);
    recipes.insert(recipeKey(EARTH, AIR),   DUST);
    recipes.insert(recipeKey(WATER, AIR),   MIST);
    recipes.insert(recipeKey(FIRE,  AIR),   STORM);
}

void Game::buildDungeon()
{
    // TREE: each room has up to two exits (left / right) plus a link back to its parent.
    int steamSeq[] = { STEAM };
    int mudSeq[]   = { MUD };
    int heartSeq[] = { STEAM, MUD, LAVA };

    Room* r0  = new Room(0,  "Entrance Hall",   new Shrine("Ancient Brazier", 5));
    Room* r1  = new Room(1,  "Ember Cavern",    new ResourceNode("Magma Seam", FIRE, 3, 2));
    Room* r2  = new Room(2,  "Mossy Tunnel",    new ResourceNode("Mossy Boulders", EARTH, 3, 1));
    Room* r3  = new Room(3,  "Flooded Crypt",   new ResourceNode("Dripping Font", WATER, 4, 2));
    Room* r4  = new Room(4,  "Ashen Hollow",    new ResourceNode("Cinder Pile", EARTH, 2, 0));
    Room* r5  = new Room(5,  "Mud Gate",        new ElementGate("Mire Ward", mudSeq, 1, false));
    Room* r6  = new Room(6,  "Warden's Hall",   new Guardian("Stone Warden", DUST));
    Room* r7  = new Room(7,  "Steam Gate",      new ElementGate("Vapour Ward", steamSeq, 1, false));
    Room* r8  = new Room(8,  "Gale Ledge",      new ResourceNode("Wind Chimes", AIR, 3, 1));
    Room* r9  = new Room(9,  "Hidden Cache",    new ResourceNode("Buried Chest", WATER, 2, 3));
    Room* r10 = new Room(10, "Heart Chamber",   new ElementGate("Heart Ward", heartSeq, 3, true));
    Room* r11 = new Room(11, "Vault of Embers", new ResourceNode("Ember Hoard", FIRE, 2, 3));
    Room* r12 = new Room(12, "Spring Alcove",   new Shrine("Healing Spring", 5));

    r0->attachLeft(r1);  r0->attachRight(r2);
    r1->attachLeft(r3);  r1->attachRight(r4);
    r3->attachLeft(r7);  r3->attachRight(r8);
    r7->attachLeft(r11); r7->attachRight(r12);
    r2->attachLeft(r5);  r2->attachRight(r6);
    r5->attachLeft(r9);
    r6->attachLeft(r10);

    root = r0;
    player.reset();
    player.location = root;
    player.note("Entered the Sunken Citadel");
}

// ---------------------------------------------------------------- output
void Game::printIntro() const
{
    std::cout << "==============================================\n"
              << "   AETHERFORGE: The Sunken Citadel\n"
              << "==============================================\n"
              << "The Heart Ward at the bottom of the citadel must be broken.\n"
              << "Gather elements, fuse compounds, and offer them in the right order.\n"
              << "Energy runs out? You collapse. Manage Energy and Essence wisely.\n\n";
}

void Game::printHelp() const
{
    std::cout << "Commands:\n"
              << "  look                 show the current room\n"
              << "  left | right | back  move through the tree (1 Energy)\n"
              << "  use [item]           harvest / rest / offer an item to a gate or guardian\n"
              << "  fuse <a> <b>         combine two items into a compound (1 Essence)\n"
              << "  inv                  show resources and inventory\n"
              << "  journal              browse your journal (n = newer, p = older, q = quit)\n"
              << "  undo                 rewind the last move/fusion (costs 1 Energy)\n"
              << "  scout                reveal the whole citadel (2 Essence)\n"
              << "  save | load          save or load progress to a file\n"
              << "  debug                dump the recipe hash table (for testing)\n"
              << "  help | quit\n";
}

void Game::printStatus() const
{
    std::cout << "[Energy " << player.energy << "/" << Player::MAX_ENERGY << " | Essence " << player.essence << "]\n";
}

void Game::printRoom()
{
    Room* r = player.location;
    std::cout << "\n=== [" << r->id << "] " << r->name << " ===\n";
    if (r->entity) std::cout << r->entity->describe() << "\n";
    else std::cout << "An empty chamber.\n";

    std::cout << "Exits: ";
    if (r->left)  std::cout << "left -> " << r->left->name << "  ";
    if (r->right) std::cout << "right -> " << r->right->name << "  ";
    if (r->parent) std::cout << "back -> " << r->parent->name;
    if (r->isBlocked() && (r->left || r->right)) std::cout << "\n(The forward exits are sealed until you deal with the obstacle here.)";
    std::cout << "\n";
}

// ---------------------------------------------------------------- commands
void Game::cmdMove(const std::string& dir)
{
    Room* cur = player.location;
    Room* target = nullptr;

    if (dir == "back")
    {
        if (!cur->parent) { std::cout << "There is no way back; you are at the entrance.\n"; return; }
        target = cur->parent;
    }
    else
    {
        if (cur->isBlocked()) { std::cout << "Something here seals the way forward!\n"; return; }
        target = (dir == "left") ? cur->left : cur->right;
        if (!target) { std::cout << "There is no passage that way.\n"; return; }
    }

    player.saveSnapshot();                 // STACK push before the state changes
    player.location = target;
    player.loseEnergy(1);
    printRoom();
}

void Game::cmdUse(const std::string& itemText)
{
    Entity* e = player.location->entity;
    if (!e) { std::cout << "There is nothing to use here.\n"; return; }

    int item = -1;
    if (!itemText.empty())
    {
        item = parseItem(itemText);
        if (item < 0) { std::cout << "Unknown item '" << itemText << "'.\n"; return; }
    }

    player.clearUndo();                    // using things is a committed action
    std::cout << e->interact(player, item) << "\n";   // POLYMORPHISM: each entity reacts differently

    ElementGate* gate = dynamic_cast<ElementGate*>(e);
    if (gate && gate->isFinal() && !gate->blocksPassage()) won = true;
}

void Game::cmdFuse(const std::string& a, const std::string& b)
{
    int ia = parseItem(a), ib = parseItem(b);
    if (ia < 0 || ib < 0) { std::cout << "Usage: fuse <item> <item>\n"; return; }
    if (player.essence < 1) { std::cout << "You need 1 Essence to fuse.\n"; return; }

    int need = (ia == ib) ? 2 : 1;
    if (player.countItem(ia) < need || player.countItem(ib) < 1)
    {
        std::cout << "You do not hold those items.\n";
        return;
    }

    int* result = recipes.find(recipeKey(ia, ib));    // HASH TABLE lookup
    if (!result) { std::cout << "No reaction. Nothing was consumed.\n"; return; }

    player.saveSnapshot();
    player.removeItem(ia, 1);
    player.removeItem(ib, 1);
    player.addItem(*result, 1);
    player.essence -= 1;
    std::string msg = std::string("Fused ") + itemName(ia) + " + " + itemName(ib) + " -> " + itemName(*result);
    player.note(msg);
    std::cout << msg << ". (-1 Essence)\n";
}

void Game::cmdInventory()
{
    printStatus();
    if (player.inventory.isEmpty()) { std::cout << "Inventory is empty.\n"; return; }
    std::cout << "Inventory:\n";
    // ITERATOR: walk the singly linked list without touching its nodes directly.
    for (SinglyLinkedList<InvSlot>::Iterator it = player.inventory.begin(); it != player.inventory.end(); ++it)
        std::cout << "  " << itemName((*it).id) << " x" << (*it).qty << "\n";
}

void Game::cmdJournal()
{
    if (player.journal.size() == 0) { std::cout << "The journal is empty.\n"; return; }

    DoublyLinkedList<std::string>::Iterator it = player.journal.last();   // start at newest
    int position = player.journal.size();
    std::string input;
    while (true)
    {
        std::cout << "(" << position << "/" << player.journal.size() << ") " << *it << "\n  [n]ewer [p]older [q]uit > ";
        if (!std::getline(std::cin, input)) return;
        input = lower(input);
        if (input == "q") return;
        if (input == "p")
        {
            if (it.hasPrev()) { --it; --position; }     // doubly linked: step BACKWARD
            else std::cout << "  (this is the oldest entry)\n";
        }
        else if (input == "n")
        {
            if (it.hasNext()) { ++it; ++position; }     // ...and FORWARD
            else std::cout << "  (this is the newest entry)\n";
        }
    }
}

void Game::cmdUndo()
{
    if (!player.rewind()) { std::cout << "Nothing to rewind (gathering/using locks in your progress).\n"; return; }
    std::cout << "Time folds back. (-1 Energy tax)\n";
    printRoom();
}

void Game::cmdScout()
{
    if (player.essence < 2) { std::cout << "Scouting needs 2 Essence.\n"; return; }
    player.essence -= 2;
    player.clearUndo();
    std::cout << "Your Essence flares and the citadel reveals itself:\n";
    ScoutVisitor scout(player.location);
    root->accept(scout);                   // VISITOR over the tree
}

void Game::cmdSave()
{
    std::ofstream out(SAVE_FILE);
    if (!out) { std::cout << "Could not open the save file.\n"; return; }

    out << "AETHERFORGE_SAVE_1\n";
    out << player.energy << " " << player.essence << " " << player.location->id << "\n";
    int counts[ITEM_COUNT];
    player.exportCounts(counts);
    for (int i = 0; i < ITEM_COUNT; ++i) out << counts[i] << (i + 1 < ITEM_COUNT ? " " : "\n");
    out << player.journal.size() << "\n";
    for (DoublyLinkedList<std::string>::Iterator it = player.journal.begin(); it != player.journal.end(); ++it)
        out << *it << "\n";
    SaveVisitor sv(out);                   // VISITOR writes every room's state
    root->accept(sv);
    std::cout << "Progress saved to " << SAVE_FILE << ".\n";
}

void Game::cmdLoad()
{
    std::ifstream in(SAVE_FILE);
    if (!in) { std::cout << "No save file found.\n"; return; }

    std::string line;
    int energy = 0, essence = 0, roomId = 0, counts[ITEM_COUNT], journalCount = 0, states[MAX_ROOMS];
    std::string entries[40];
    for (int i = 0; i < MAX_ROOMS; ++i) states[i] = 0;

    // Parse everything into temporaries FIRST so a bad file cannot corrupt the live game.
    if (!std::getline(in, line) || line != "AETHERFORGE_SAVE_1") { std::cout << "Save file is not valid.\n"; return; }
    { std::getline(in, line); std::istringstream s(line); if (!(s >> energy >> essence >> roomId)) { std::cout << "Save file is corrupted.\n"; return; } }
    { std::getline(in, line); std::istringstream s(line); for (int i = 0; i < ITEM_COUNT; ++i) if (!(s >> counts[i])) { std::cout << "Save file is corrupted.\n"; return; } }
    { std::getline(in, line); std::istringstream s(line); if (!(s >> journalCount) || journalCount < 0 || journalCount > 40) { std::cout << "Save file is corrupted.\n"; return; } }
    for (int i = 0; i < journalCount; ++i)
        if (!std::getline(in, entries[i])) { std::cout << "Save file is corrupted.\n"; return; }
    while (std::getline(in, line))
    {
        std::istringstream s(line);
        int id, st;
        if (s >> id >> st) if (id >= 0 && id < MAX_ROOMS) states[id] = st;
    }
    if (roomId < 0 || roomId >= MAX_ROOMS) { std::cout << "Save file is corrupted.\n"; return; }

    // Rebuild a fresh dungeon, then overlay the saved state.
    delete root;
    buildDungeon();
    player.journal.clear();
    player.energy = energy;
    player.essence = essence;
    player.importCounts(counts);
    for (int i = 0; i < journalCount; ++i) player.note(entries[i]);

    ApplyStateVisitor apply(states);
    root->accept(apply);
    FindRoomVisitor finder(roomId);
    root->accept(finder);
    player.location = finder.found ? finder.found : root;
    player.clearUndo();

    std::cout << "Progress loaded.\n";
    printRoom();
}

// ---------------------------------------------------------------- loop
void Game::handle(const std::string& line)
{
    std::istringstream in(line);
    std::string cmd, a, b;
    in >> cmd >> a >> b;
    cmd = lower(cmd);

    if (cmd.empty()) return;
    else if (cmd == "look") printRoom();
    else if (cmd == "left" || cmd == "right" || cmd == "back") cmdMove(cmd);
    else if (cmd == "use" || cmd == "gather" || cmd == "offer" || cmd == "rest") cmdUse(a);
    else if (cmd == "fuse") cmdFuse(a, b);
    else if (cmd == "inv" || cmd == "inventory") cmdInventory();
    else if (cmd == "journal") cmdJournal();
    else if (cmd == "undo") cmdUndo();
    else if (cmd == "scout") cmdScout();
    else if (cmd == "save") cmdSave();
    else if (cmd == "load") cmdLoad();
    else if (cmd == "debug") { std::cout << "Recipe hash table (" << recipes.size() << " entries):\n"; recipes.dump(); }
    else if (cmd == "help") printHelp();
    else if (cmd == "quit" || cmd == "exit") running = false;
    else std::cout << "Unknown command. Type 'help'.\n";
}

void Game::checkEnd()
{
    if (won)
    {
        std::cout << "\n*** The Heart Ward shatters. The Sunken Citadel is free. YOU WIN! ***\n";
        running = false;
    }
    else if (player.energy <= 0)
    {
        std::cout << "\nYour strength fails and the citadel claims you. GAME OVER.\n";
        running = false;
    }
}

void Game::run()
{
    printIntro();
    printHelp();
    printRoom();

    std::string line;
    while (running)
    {
        printStatus();
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        handle(line);
        checkEnd();
    }
}

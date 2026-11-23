#include "Player.h"

Player::Player() { reset(); }

void Player::reset()
{
    energy = 20;
    essence = 0;
    location = nullptr;
    inventory.clear();
    journal.clear();
    undoStack.clear();
}

void Player::addItem(int id, int qty)
{
    if (qty <= 0) return;
    // Walk the list with the Iterator; stack onto an existing slot if present.
    for (SinglyLinkedList<InvSlot>::Iterator it = inventory.begin(); it != inventory.end(); ++it)
    {
        if ((*it).id == id) { (*it).qty += qty; return; }
    }
    InvSlot slot; slot.id = id; slot.qty = qty;
    inventory.pushBack(slot);
}

bool Player::removeItem(int id, int qty)
{
    for (SinglyLinkedList<InvSlot>::Iterator it = inventory.begin(); it != inventory.end(); ++it)
    {
        if ((*it).id == id)
        {
            if ((*it).qty < qty) return false;
            (*it).qty -= qty;
            if ((*it).qty == 0)                    // empty slot: unlink the node
                inventory.removeIf([id](const InvSlot& s) { return s.id == id; });
            return true;
        }
    }
    return false;
}

int Player::countItem(int id) const
{
    for (SinglyLinkedList<InvSlot>::Iterator it = inventory.begin(); it != inventory.end(); ++it)
        if ((*it).id == id) return (*it).qty;
    return 0;
}

void Player::gainEnergy(int amount)
{
    energy += amount;
    if (energy > MAX_ENERGY) energy = MAX_ENERGY;
}

void Player::loseEnergy(int amount)
{
    energy -= amount;
    if (energy < 0) energy = 0;
}

void Player::note(const std::string& text)
{
    journal.pushBack(text);
    while (journal.size() > MAX_JOURNAL) journal.popFront();
}

void Player::exportCounts(int out[]) const
{
    for (int i = 0; i < ITEM_COUNT; ++i) out[i] = 0;
    for (SinglyLinkedList<InvSlot>::Iterator it = inventory.begin(); it != inventory.end(); ++it)
        out[(*it).id] = (*it).qty;
}

void Player::importCounts(const int in[])
{
    inventory.clear();
    for (int i = 0; i < ITEM_COUNT; ++i) addItem(i, in[i]);
}

void Player::saveSnapshot()
{
    Snapshot s;
    s.room = location;
    s.energy = energy;
    s.essence = essence;
    exportCounts(s.counts);
    undoStack.push(s);                             // STACK: push
}

bool Player::rewind()
{
    Snapshot s;
    if (!undoStack.pop(s)) return false;           // STACK: pop (LIFO = most recent action first)
    location = s.room;
    energy = s.energy;
    essence = s.essence;
    importCounts(s.counts);
    loseEnergy(1);                                 // time-travel tax
    return true;
}

void Player::clearUndo() { undoStack.clear(); }

#include "Entity.h"
#include "Player.h"
#include <sstream>

// ---------- ResourceNode ----------
ResourceNode::ResourceNode(const std::string& n, int item, int qty, int essence)
    : Entity(n), itemId(item), itemQty(qty), essenceQty(essence), depleted(false) {}

std::string ResourceNode::describe() const
{
    std::ostringstream o;
    o << name << ": ";
    if (depleted) o << "drained.";
    else
    {
        o << "holds " << itemQty << " " << itemName(itemId);
        if (essenceQty > 0) o << " + " << essenceQty << " Essence";
        o << ". Type 'use' to harvest (1 Energy).";
    }
    return o.str();
}

std::string ResourceNode::summary() const
{
    std::ostringstream o;
    o << "Node: ";
    if (depleted) o << "drained";
    else { o << itemQty << " " << itemName(itemId); if (essenceQty > 0) o << " + " << essenceQty << " Essence"; }
    return o.str();
}

std::string ResourceNode::interact(Player& p, int)
{
    if (depleted) return "Nothing left to harvest here.";
    p.loseEnergy(1);
    p.addItem(itemId, itemQty);
    p.essence += essenceQty;
    depleted = true;
    p.note("Harvested " + name);
    std::ostringstream o;
    o << "You harvest " << itemQty << " " << itemName(itemId);
    if (essenceQty > 0) o << " and " << essenceQty << " Essence";
    o << ". (-1 Energy)";
    return o.str();
}

// ---------- Shrine ----------
Shrine::Shrine(const std::string& n, int restore) : Entity(n), amount(restore), used(false) {}

std::string Shrine::describe() const
{
    std::ostringstream o;
    o << name << ": ";
    if (used) o << "its flame has gone out.";
    else o << "a warm flame. 'use' to restore " << amount << " Energy (one time).";
    return o.str();
}

std::string Shrine::summary() const { return used ? "Shrine: spent" : "Shrine: restores Energy"; }

std::string Shrine::interact(Player& p, int)
{
    if (used) return "The flame is already spent.";
    p.gainEnergy(amount);
    used = true;
    p.note("Rested at " + name);
    std::ostringstream o;
    o << "You warm your hands. (+" << amount << " Energy)";
    return o.str();
}

// ---------- Guardian ----------
Guardian::Guardian(const std::string& n, int tributeItem) : Entity(n), tribute(tributeItem), pacified(false) {}

std::string Guardian::describe() const
{
    std::ostringstream o;
    o << name << ": ";
    if (pacified) o << "calm now. The path is open.";
    else o << "a stone sentinel blocks the way. It craves a tribute of " << itemName(tribute) << ". Use: 'use <item>'.";
    return o.str();
}

std::string Guardian::summary() const
{
    return pacified ? "Guardian: calm" : std::string("Guardian: wants ") + itemName(tribute);
}

std::string Guardian::interact(Player& p, int itemId)
{
    if (pacified) return "The guardian is already calm.";
    if (itemId < 0) return "Offer something: use <item>.";
    if (p.countItem(itemId) < 1) return std::string("You do not have any ") + itemName(itemId) + ".";
    if (itemId != tribute)
    {
        p.loseEnergy(3);                                   // wrong guess hurts
        return std::string("The guardian rejects ") + itemName(itemId) + " and strikes you! (-3 Energy)";
    }
    p.removeItem(itemId, 1);
    pacified = true;
    p.note("Pacified " + name + " with " + itemName(itemId));
    return std::string("The guardian accepts the ") + itemName(itemId) + " and steps aside.";
}

// ---------- ElementGate ----------
ElementGate::ElementGate(const std::string& n, const int* seq, int len, bool isFinal)
    : Entity(n), total(len), finalGate(isFinal)
{
    for (int i = 0; i < len && i < 4; ++i) sequence[i] = seq[i];
    rebuild(0);
}

void ElementGate::rebuild(int alreadyDone)
{
    pending.clear();
    for (int i = alreadyDone; i < total; ++i) pending.enqueue(sequence[i]);   // ENQUEUE in required order
}

std::string ElementGate::describe() const
{
    std::ostringstream o;
    o << name << ": ";
    if (pending.isEmpty()) { o << "the ward has dissolved."; return o.str(); }
    o << "a ward demands compounds IN ORDER: ";
    for (int i = 0; i < pending.size(); ++i)
    {
        if (i > 0) o << " -> ";
        o << itemName(pending.at(i));
    }
    o << ". Use: 'use <item>'.";
    return o.str();
}

std::string ElementGate::summary() const
{
    if (pending.isEmpty()) return "Gate: open";
    std::ostringstream o;
    o << (finalGate ? "FINAL Gate: " : "Gate: ");
    for (int i = 0; i < pending.size(); ++i) { if (i > 0) o << " -> "; o << itemName(pending.at(i)); }
    return o.str();
}

std::string ElementGate::interact(Player& p, int itemId)
{
    if (pending.isEmpty()) return "The gate is already open.";
    if (itemId < 0) return "Offer a compound: use <item>.";
    if (p.countItem(itemId) < 1) return std::string("You do not have any ") + itemName(itemId) + ".";

    int needed;
    pending.front(needed);                                 // QUEUE: peek at the next required item
    if (itemId != needed)
    {
        p.loseEnergy(1);
        return std::string("The ward flickers and rejects ") + itemName(itemId) + ". (-1 Energy)";
    }
    p.removeItem(itemId, 1);
    int done;
    pending.dequeue(done);                                 // QUEUE: dequeue = requirement satisfied
    p.note(std::string("Offered ") + itemName(itemId) + " to " + name);
    if (pending.isEmpty()) return std::string("The final seal breaks. ") + name + " dissolves!";
    return std::string("The ward accepts ") + itemName(itemId) + ". The next seal glows...";
}

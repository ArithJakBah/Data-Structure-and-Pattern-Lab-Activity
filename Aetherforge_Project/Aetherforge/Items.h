// Items.h - item identifiers and helpers shared by the whole game.
#pragma once
#include <string>
#include <cctype>

// 4 base elements (gathered) + 6 compounds (fused). Compounds open gates.
enum ItemId { FIRE = 0, WATER, EARTH, AIR, STEAM, MUD, LAVA, DUST, MIST, STORM, ITEM_COUNT };

inline const char* itemName(int id)
{
    static const char* names[ITEM_COUNT] = { "Fire", "Water", "Earth", "Air", "Steam", "Mud", "Lava", "Dust", "Mist", "Storm" };
    return (id >= 0 && id < ITEM_COUNT) ? names[id] : "Unknown";
}

// Case-insensitive name -> id. Returns -1 when the name is not an item.
inline int parseItem(const std::string& text)
{
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        std::string n = itemName(i);
        if (n.size() != text.size()) continue;
        bool same = true;
        for (size_t k = 0; k < n.size(); ++k)
            if (std::tolower((unsigned char)n[k]) != std::tolower((unsigned char)text[k])) { same = false; break; }
        if (same) return i;
    }
    return -1;
}

// Order-independent hash-table key: Fire+Water and Water+Fire give the same key.
inline std::string recipeKey(int a, int b)
{
    if (a > b) { int t = a; a = b; b = t; }
    return std::string(itemName(a)) + "+" + itemName(b);
}

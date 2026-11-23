// HashTable.h - CONCEPT 2a: Hash Table (separate chaining), built by hand.
// Used by the alchemy system: key "Fire+Water" -> value (item id of Steam).
#pragma once
#include <string>
#include <iostream>

template <typename V>
class HashTable
{
private:
    struct Entry
    {
        std::string key;
        V value;
        Entry* next;                      // chain of colliding keys
        Entry(const std::string& k, const V& v, Entry* n) : key(k), value(v), next(n) {}
    };

    Entry** buckets;
    int capacity;
    int count;

    // djb2 string hash, folded into the bucket range with modulo.
    int hashKey(const std::string& key) const
    {
        unsigned long h = 5381;
        for (size_t i = 0; i < key.size(); ++i)
            h = ((h << 5) + h) + (unsigned char)key[i];
        return (int)(h % (unsigned long)capacity);
    }

public:
    explicit HashTable(int cap = 11) : capacity(cap), count(0)
    {
        buckets = new Entry*[capacity];
        for (int i = 0; i < capacity; ++i) buckets[i] = nullptr;
    }

    ~HashTable()
    {
        for (int i = 0; i < capacity; ++i)
        {
            Entry* e = buckets[i];
            while (e) { Entry* n = e->next; delete e; e = n; }
        }
        delete[] buckets;
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Insert, or overwrite when the key already exists. Returns true if new.
    bool insert(const std::string& key, const V& value)
    {
        int idx = hashKey(key);
        for (Entry* e = buckets[idx]; e; e = e->next)
            if (e->key == key) { e->value = value; return false; }
        buckets[idx] = new Entry(key, value, buckets[idx]);   // push onto the chain head
        ++count;
        return true;
    }

    // Returns a pointer to the stored value, or nullptr if the key is absent.
    V* find(const std::string& key)
    {
        for (Entry* e = buckets[hashKey(key)]; e; e = e->next)
            if (e->key == key) return &e->value;
        return nullptr;
    }

    int size() const { return count; }

    // Testing output for the report: shows which bucket each key landed in.
    void dump() const
    {
        for (int i = 0; i < capacity; ++i)
        {
            std::cout << "  bucket " << i << ": ";
            for (Entry* e = buckets[i]; e; e = e->next) std::cout << "[" << e->key << " -> " << e->value << "] ";
            std::cout << "\n";
        }
    }
};

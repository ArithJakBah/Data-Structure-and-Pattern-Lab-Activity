#pragma once
#include <cstddef>
#include "SinglyLinkedList.h"
#include "Item.h"

// Other code walks the inventory with this, without knowing it is a linked list.
using ItemIterator = SinglyLinkedList<Item*>::Iterator;

// The bag. Items are kept in a singly linked list, in the order they were picked up.
// The inventory OWNS the items it holds and deletes them when it is destroyed.
class Inventory
{
public:
	static constexpr size_t CAPACITY = 8;

private:
	SinglyLinkedList<Item*> fItems;

public:
	Inventory() = default;
	~Inventory();

	Inventory(const Inventory&) = delete;
	Inventory& operator=(const Inventory&) = delete;

	bool   add(Item* aItem);                // false when the bag is full (or the item is null)
	bool   remove(Item* aItem);             // takes it out WITHOUT deleting it
	size_t count() const noexcept;
	bool   isFull() const noexcept;

	ItemIterator begin() const noexcept;
	ItemIterator end() const noexcept;

	Item* at(size_t aIndex) const;          // walks the list with the iterator
	int   countKeyFragments() const;
};

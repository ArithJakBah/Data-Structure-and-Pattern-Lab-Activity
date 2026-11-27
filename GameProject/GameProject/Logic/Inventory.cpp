#include "Inventory.h"

Inventory::~Inventory()
{
	for (ItemIterator it = begin(); it != end(); ++it)
	{
		delete *it;
	}
}

bool Inventory::add(Item* aItem)
{
	if (aItem == nullptr || isFull())
	{
		return false;
	}
	fItems.pushBack(aItem);
	return true;
}

bool Inventory::remove(Item* aItem)
{
	return fItems.remove(aItem);
}

size_t Inventory::count() const noexcept
{
	return fItems.size();
}

bool Inventory::isFull() const noexcept
{
	return fItems.size() >= CAPACITY;
}

ItemIterator Inventory::begin() const noexcept
{
	return fItems.begin();
}

ItemIterator Inventory::end() const noexcept
{
	return fItems.end();
}

Item* Inventory::at(size_t aIndex) const
{
	size_t position = 0;
	for (ItemIterator it = begin(); it != end(); ++it)
	{
		if (position == aIndex)
		{
			return *it;
		}
		position++;
	}
	return nullptr;
}

int Inventory::countKeyFragments() const
{
	int total = 0;
	for (ItemIterator it = begin(); it != end(); ++it)
	{
		if ((*it)->isKeyFragment())
		{
			total++;
		}
	}
	return total;
}

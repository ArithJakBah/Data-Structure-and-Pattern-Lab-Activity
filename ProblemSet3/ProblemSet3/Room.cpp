// Implementation of the Room node class (see Room.h).
#include "Room.h"
#include <algorithm>

// A new room starts with a name, no entities and no doors.
Room::Room(const std::string& aName) : fName(aName) {}

const std::string& Room::name() const noexcept { return fName; }

// std::move transfers ownership of the entity into the vector, because a unique_ptr cannot be copied.
void Room::addEntity(std::unique_ptr<Entity> aEntity)
{
    fEntities.push_back(std::move(aEntity));
}

const std::vector<std::unique_ptr<Entity>>& Room::entities() const noexcept
{
    return fEntities;
}

// offer every entity to the visitor (accept() picks the right visit() overload). 
// then erase the entities that report they are gone.
void Room::apply(Visitor& aVisitor)
{
    for (const auto& entity : fEntities)
    {
        entity->accept(aVisitor);
    }

    fEntities.erase(
        std::remove_if(fEntities.begin(), fEntities.end(),
            [](const std::unique_ptr<Entity>& e) { return e->isGone(); }),
        fEntities.end());
}

// Creates the child on the heap, stores it as a new door and returns it.
// Heap memory location keeps each room at a fixed address for its whole life.
Room& Room::addChild(const std::string& aName)
{
    fChildren.push_back(std::make_unique<Room>(aName));
    return *fChildren.back();
}

size_t Room::doorCount() const noexcept { return fChildren.size(); }

// Bounds check so an invalid door number returns nullptr instead of reading outside the vector.
Room* Room::door(size_t aIndex) const noexcept
{
    if (aIndex >= fChildren.size()) return nullptr;
    return fChildren[aIndex].get();
}
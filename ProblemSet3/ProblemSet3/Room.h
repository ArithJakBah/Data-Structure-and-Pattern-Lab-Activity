// Each Room is one node. It stores the entities inside it (monsters, traps, items) and the child rooms reachable through its doors.

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Entity.h"
#include "Visitor.h"

class Room
{
private:
    std::string fName;  // name of the room

    // Everything inside the room. Pointers to the Entity base class are used so one vector can hold Monsters, Traps and Items.
    std::vector<std::unique_ptr<Entity>> fEntities; 

    // The room owns its children. The doors leading out of this room. Each child is another Room.
    std::vector<std::unique_ptr<Room>> fChildren;

public:
    explicit Room(const std::string& aName); //Creates an empty room with the given name.

    const std::string& name() const noexcept; //Returns the room's name.

    void addEntity(std::unique_ptr<Entity> aEntity); //Puts an entity into the room. The room takes ownership of it.
    const std::vector<std::unique_ptr<Entity>>& entities() const noexcept;

    //run a visitor (a player action) over every entity in the room,
    //then remove the entities that are used up (dead, disarmed, collected)
    void apply(Visitor& aVisitor);

    Room& addChild(const std::string& aName); //create a child room then return it
    size_t doorCount() const noexcept; //Number of doors leading out of this room
    Room* door(size_t aIndex) const noexcept; //nullptr if index is invalid
};
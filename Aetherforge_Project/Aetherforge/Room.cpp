#include "Room.h"

Room::Room(int i, const std::string& n, Entity* e)
    : id(i), name(n), left(nullptr), right(nullptr), parent(nullptr), entity(e) {}

Room::~Room()
{
    delete left;                          // post-order delete of the subtree
    delete right;
    delete entity;
}

void Room::attachLeft(Room* child)  { left = child;  child->parent = this; }
void Room::attachRight(Room* child) { right = child; child->parent = this; }

bool Room::isBlocked() const { return entity && entity->blocksPassage(); }

int Room::depth() const
{
    int d = 0;
    for (const Room* r = parent; r; r = r->parent) ++d;
    return d;
}

void Room::accept(RoomVisitor& v)
{
    v.visit(*this);                       // 1) this node
    if (left)  left->accept(v);           // 2) left subtree
    if (right) right->accept(v);          // 3) right subtree
}

void ScoutVisitor::visit(Room& room)
{
    for (int i = 0; i < room.depth(); ++i) std::cout << "    ";
    std::cout << (&room == current ? "* " : "- ") << "[" << room.id << "] " << room.name;
    if (room.entity) std::cout << "  <" << room.entity->summary() << ">";
    std::cout << "\n";
}

void SaveVisitor::visit(Room& room)
{
    out << room.id << " " << (room.entity ? room.entity->saveState() : 0) << "\n";
}

void ApplyStateVisitor::visit(Room& room)
{
    if (room.entity && room.id >= 0 && room.id < MAX_ROOMS) room.entity->loadState(states[room.id]);
}

void FindRoomVisitor::visit(Room& room)
{
    if (room.id == target) found = &room;
}

// DoublyLinkedList.h - CONCEPT 2c: Doubly Linked-List (+ bidirectional Iterator).
// Used for the Player's Journal, which can be browsed forwards AND backwards.
#pragma once

template <typename T>
class DoublyLinkedList
{
public:
    struct Node
    {
        T data;
        Node* prev;
        Node* next;
        explicit Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

    class Iterator
    {
        Node* cur;
    public:
        explicit Iterator(Node* n = nullptr) : cur(n) {}
        T& operator*() const { return cur->data; }
        Iterator& operator++() { if (cur) cur = cur->next; return *this; }
        Iterator& operator--() { if (cur) cur = cur->prev; return *this; }
        bool operator!=(const Iterator& o) const { return cur != o.cur; }
        bool hasNext() const { return cur && cur->next; }
        bool hasPrev() const { return cur && cur->prev; }
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    void pushBack(const T& value)
    {
        Node* n = new Node(value);
        n->prev = tail;                    // both directions must be linked
        if (tail) tail->next = n; else head = n;
        tail = n;
        ++count;
    }

    // Drops the oldest entry (used to cap the journal length).
    bool popFront()
    {
        if (!head) return false;
        Node* old = head;
        head = head->next;
        if (head) head->prev = nullptr; else tail = nullptr;
        delete old;
        --count;
        return true;
    }

    void clear() { while (popFront()) {} }

    int size() const { return count; }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
    Iterator last() const { return Iterator(tail); }
};

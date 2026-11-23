// SinglyLinkedList.h - CONCEPT 2b: Singly Linked-List + CONCEPT 4a: Iterator.
// Used for the Player's inventory (a list of item slots).
#pragma once

template <typename T>
class SinglyLinkedList
{
public:
    struct Node
    {
        T data;
        Node* next;
        explicit Node(const T& d) : data(d), next(nullptr) {}
    };

    // ITERATOR PATTERN: lets callers walk the list without knowing about Nodes.
    class Iterator
    {
        Node* cur;
    public:
        explicit Iterator(Node* n = nullptr) : cur(n) {}
        T& operator*() const { return cur->data; }
        Iterator& operator++() { if (cur) cur = cur->next; return *this; }
        bool operator!=(const Iterator& o) const { return cur != o.cur; }
        bool operator==(const Iterator& o) const { return cur == o.cur; }
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    SinglyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~SinglyLinkedList() { clear(); }
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    void pushBack(const T& value)
    {
        Node* n = new Node(value);
        if (tail) tail->next = n; else head = n;
        tail = n;
        ++count;
    }

    // Removes the first node whose data satisfies the predicate.
    template <typename Pred>
    bool removeIf(Pred pred)
    {
        Node* prev = nullptr;
        Node* c = head;
        while (c)
        {
            if (pred(c->data))
            {
                if (prev) prev->next = c->next; else head = c->next;   // unlink
                if (c == tail) tail = prev;
                delete c;
                --count;
                return true;
            }
            prev = c;
            c = c->next;
        }
        return false;
    }

    void clear()
    {
        while (head) { Node* n = head->next; delete head; head = n; }
        tail = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
};

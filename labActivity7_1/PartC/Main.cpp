#include <iostream>
#include "DoublyLinkedNode.h"
#include "DoublyLinkedNodeIterator.h"
#include "List.h"
#include "Stack.h"
#include "Queue.h"
using namespace std;

int main()
{
    // ---- Stack test (max 3) ----
    Stack<int> s(3);
    for (int i = 1; i <= 4; ++i)
    {
        if (s.push(i)) cout << "Pushed " << i << endl;
        else           cout << "Stack full, could not push " << i << endl;
    }
    while (!s.isEmpty())
        cout << "Popped: " << s.pop() << endl;      // 3 2 1

    cout << endl;

    // ---- Queue test (max 3) ----
    Queue<string> q(3);
    string items[] = { "A", "B", "C", "D" };

    for (const string& item : items)
    {
        if (q.enqueue(item)) cout << "Enqueued " << item << endl;
        else                 cout << "Queue full, could not enqueue " << item << endl;
    }

    while (!q.isEmpty())
        cout << "Dequeued: " << q.dequeue() << endl;

   
    return 0;
}
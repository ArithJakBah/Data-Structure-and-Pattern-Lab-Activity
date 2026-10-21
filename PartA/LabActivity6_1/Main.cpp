#include <iostream>
#include "DoublyLinkedNode.h"
#include "DoublyLinkedNodeIterator.h"
#include "List.h"

using namespace std;

void print(const List<int>& l)
{
    for (int i = 0; i < l.size(); ++i)
    {
        cout << l[i] << " ";
    }
    cout << endl;
}

int main()
{
    List<int> l;
    l.push(10); 
    l.push(20); 
    l.push(30);
    print(l);                         // 10 20 30

    l.pushFront(5);
    print(l);                         // 5 10 20 30

    l.insert(2, 15);
    print(l);                         // 5 10 15 20 30

    cout << l.removeAt(1) << endl;    // 10
    print(l);                         // 5 15 20 30

    cout << l.pop() << endl;          // 30
    print(l);                         // 5 15 20

    cout << l.popFront() << endl;     // 5
    print(l);                         // 15 20

    // walks backwards from last
    for (auto it = l.getIterator(); it != it.end(); --it)
    {
        cout << *it << " ";           // 20 15
    }
    cout << endl;

    return 0;
}
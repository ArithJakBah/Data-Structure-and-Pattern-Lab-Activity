#include <iostream>
#include <string>
#include "NTree.h"

int main()
{
	NTree<int, 3> root(1);
	NTree<int, 3>* child1 = new NTree<int, 3>(2);
	NTree<int, 3>* child2 = new NTree<int, 3>(3);

	root.attachNTree(0, child1);
	root.attachNTree(1, child2);

	std::cout << "Root key: " << root.getKey() << std::endl;
	std::cout << "Child 1 key: " << root[0].getKey() << std::endl;
	std::cout << "Child 2 key: " << root[1].getKey() << std::endl;
	std::cout << "Child 3 is NIL: " << root[2].isEmpty() << std::endl;

	// Test getSize()
	std::cout << "Total tree size: " << root.getSize() << std::endl;

	// Test search()
	NTree<int, 3>* target = root.search(4);
	if (target != nullptr)
	{
		std::cout << "Found node with key: " << target->getKey() << std::endl;
	}
	else
	{
		std::cout << "Key not found!" << std::endl;
	}

	return 0;
}
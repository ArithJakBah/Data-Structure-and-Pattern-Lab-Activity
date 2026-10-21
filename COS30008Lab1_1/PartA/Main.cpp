#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	cout << "Hello World" << endl << "\n";

	for (int i = 0; i < argc; i++)
		cout << argv[i] << endl;

	return 0;
}
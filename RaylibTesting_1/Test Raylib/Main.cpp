#include <raylib.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])

{
	InitWindow(800, 450, "my raylib window");
	
	cout << "what";


	while (!WindowShouldClose())
	{
		//cout << "Huh";
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("Hello, raylib!", 190, 200, 20, BLACK);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

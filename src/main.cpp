#include "Ship.h"
#include <raylib.h>
#include <string>

int main()
{
	InitWindow(1280, 720, "TopDog");

	Ship ship;

	while (!WindowShouldClose())
	{
		ship.update(GetFrameTime());
		BeginDrawing();
			ClearBackground(BLACK);
			DrawFPS(10, 10);
			// draw dt
			//DrawText(std::to_string(GetFrameTime()).c_str(), 10, 30, 20, WHITE);
			ship.draw();
		EndDrawing();
	}

	CloseWindow();
}

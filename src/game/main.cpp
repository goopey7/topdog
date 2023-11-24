#include "Game.h"
#include <raylib.h>
#include <string>

int main()
{
	Game game;
	InitWindow(1280, 720, "TopDog");
	game.init();

	while (!WindowShouldClose())
	{
		game.update(GetFrameTime());

		BeginDrawing();
		ClearBackground(BLACK);
		game.draw();
		EndDrawing();
	}

	CloseWindow();
}

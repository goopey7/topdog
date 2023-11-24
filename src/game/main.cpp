#include "Game.h"
#include <raylib.h>
#include <string>

int main()
{
	Game game;
	SetWindowState(FLAG_WINDOW_RESIZABLE);
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

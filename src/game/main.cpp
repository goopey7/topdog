#include "Game.h"
#include <raylib.h>
#include <string>
#include <rlImGui.h>

int main()
{
	Game game;
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "TopDog");
	game.init();
	std::thread clientListener = std::thread(&Game::listenToServer, &game);
	rlImGuiSetup(true);

	while (!WindowShouldClose())
	{
		SetExitKey(KEY_NULL);
		game.update(GetFrameTime());

		BeginDrawing();
		ClearBackground(BLACK);
		rlImGuiBegin();
		game.draw();
		rlImGuiEnd();
		EndDrawing();
	}

	game.shutdown();
	rlImGuiShutdown();
	CloseWindow();
}

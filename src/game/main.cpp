#include "Game.h"
#include <raylib.h>
#include <string>
#include <rlImGui.h>
#include <iostream>

int main()
{
	Game game;
	SetTraceLogLevel(LOG_NONE);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "TopDog");
	game.init();
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

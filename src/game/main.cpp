#include "Game.h"
#include <raylib.h>
#include <string>
#include <rlImGui.h>
#include <iostream>
#include <imgui.h>

int main()
{
	Game game;
	SetTraceLogLevel(LOG_NONE);
	InitWindow(1280, 720, "TopDog");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	game.init();
	rlImGuiSetup(true);
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

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

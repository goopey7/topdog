// Sam Collier 2023

#include "Menu.h"
#include <string>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void Menu::init() {}

void Menu::update(float dt)
{
	gui();
}

void Menu::draw() {
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("Menu", 100, 100, 20, WHITE);
	EndDrawing();
}

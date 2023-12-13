// Sam Collier 2023

#include "Game.h"
#include "Level.h"
#include "Menu.h"

#include <raygui.h>
#include <raylib.h>

void Game::init()
{
	scenes.push(std::make_unique<Menu>([this]() { this->mainMenu(); }));
	scenes.front()->init();
}

void Game::update(float dt) { scenes.front()->update(dt); }

void Game::draw() { scenes.front()->draw(); }

void Game::mainMenu()
{
	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
	if (GuiButton({GetRenderWidth() / 2.f - 100.f, GetRenderHeight() / 2.f - 50.f, 200.f, 100.f},
				  "Play"))
	{
		client.connectToServer("127.0.0.1", 4916);
		scenes.push(std::make_unique<Level>(&scenes));
		scenes.pop();
		scenes.front()->init();
	}
}

Game::~Game()
{
	client.closeConnection();
}


// Sam Collier 2023

#include "Game.h"
#include "Level.h"
#include "Menu.h"

#include <raygui.h>
#include <raylib.h>

void Game::init()
{
	scenes.push(std::make_unique<Menu>([this]() { this->mainMenu(); }));
	scenes.push(std::make_unique<Menu>([this]() { this->lobbyMenu(); }));
	scenes.push(std::make_unique<Level>(&scenes));
	scenes.front()->init();

	GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
}

void Game::update(float dt) { scenes.front()->update(dt); }

void Game::draw() { scenes.front()->draw(); }

void Game::nextScene()
{
	scenes.pop();
	scenes.front()->init();
}

void Game::mainMenu()
{
	if (GuiButton({GetRenderWidth() / 2.f - 100.f, GetRenderHeight() / 2.f - 50.f, 200.f, 100.f},
				  "Connect"))
	{
		client.connectToServer("127.0.0.1", 4916);
		nextScene();
	}
}

void Game::lobbyMenu()
{
	if (GuiButton({GetRenderWidth() / 2.f - 100.f, GetRenderHeight() / 2.f - 50.f, 200.f, 100.f},
				  "Ready"))
	{
		client.toggleReady();
	}
}

Game::~Game()
{
	if (client.isConnected())
	{
		client.closeConnection();
	}
}

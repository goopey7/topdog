// Sam Collier 2023

#include "Game.h"
#include "Level.h"

void Game::init()
{
	scenes.push(std::make_unique<Level>(&scenes));
	scenes.front()->init();
}

void Game::update(float dt)
{
	scenes.front()->update(dt);
}

void Game::draw()
{
	scenes.front()->draw();
}

// Sam Collier 2023
#pragma once

#include "Scene.h"
#include <memory>
#include <queue>

class Game
{
  public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game() = default;
	void init();
	void update(float dt);
	void draw();
  private:
	std::queue<std::unique_ptr<Scene>> scenes;
	void mainMenu();
};

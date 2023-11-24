// Sam Collier 2023
#pragma once
#include "Ship.h"

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
	Ship ship;
	Ship enemy;
};

// Sam Collier 2023

#include "Game.h"

void Game::init()
{
	ship.init(true);
	enemy.init(false);
}

void Game::update(float dt)
{
	// check collisions
	for (const Bullet& bullet : ship.getBullets())
	{
		if (CheckCollisionCircleRec(bullet.getPosition(), bullet.getSize(),
									enemy.getCollisionRect()))
		{
			enemy.onCollision(bullet);
		}
	}
	enemy.update(dt);
	ship.update(dt);
}

void Game::draw()
{
	// DrawFPS(10, 10);
	//  draw dt
	//  DrawText(std::to_string(GetFrameTime()).c_str(), 10, 30, 20, WHITE);
	enemy.draw();
	ship.draw();
}

// Sam Collier 2023

#include "Level.h"

void Level::init()
{
	ship.init(true);
	enemy.init(false);
}

void Level::update(float dt)
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

void Level::draw()
{
	// DrawFPS(10, 10);
	//  draw dt
	//  DrawText(std::to_string(GetFrameTime()).c_str(), 10, 30, 20, WHITE);
	enemy.draw();
	ship.draw();
}

void Level::transitionToMainMenu()
{
	scenes->pop();
}

Level::Level(std::queue<std::unique_ptr<Scene>>* scenes, Client* client, const std::vector<Client>* clients)
	: scenes(scenes), client(client), clients(clients)
{
}


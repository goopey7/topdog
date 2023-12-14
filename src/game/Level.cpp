// Sam Collier 2023

#include "Level.h"

void Level::init()
{
	ship.init(true, client->getName());
	for (const Client& c : *otherClients)
	{
		Ship otherShip;
		otherShip.init(false, c.getName());
		otherShips.push_back(otherShip);
	}
}

void Level::update(float dt)
{
	// check collisions
	for (const Bullet& bullet : ship.getBullets())
	{
		for (Ship& otherShip : otherShips)
		{
			if (CheckCollisionCircleRec(bullet.getPosition(), bullet.getSize(),
										otherShip.getCollisionRect()))
			{
				otherShip.onCollision(bullet);
			}
		}
	}

	// update all ships
	for (Ship& otherShip : otherShips)
	{
		otherShip.update(dt);
	}
	ship.update(dt);

	// erase disconnected ships
	if (otherShips.size() != otherClients->size())
	{
		for (Ship& s : otherShips)
		{
			bool found = false;
			for (const Client& c : *otherClients)
			{
				if (s.getName() == c.getName())
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				otherShips.erase(otherShips.begin());
				break;
			}
		}
	}
}

void Level::draw()
{
	for (Ship& otherShip : otherShips)
	{
		otherShip.draw();
	}
	ship.draw();
}

void Level::transitionToMainMenu()
{
	scenes->pop();
}

Level::Level(std::queue<std::unique_ptr<Scene>>* scenes, Client* client, const std::vector<Client>* clients)
	: scenes(scenes), client(client), otherClients(clients)
{
}


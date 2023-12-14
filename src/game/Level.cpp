// Sam Collier 2023

#include "Level.h"

void Level::init()
{
	ship.init(true);
	for (const Client& c : *otherClients)
	{
		Ship otherShip(c.getName());
		otherShip.init(false);
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
	for (Ship& otherShip : otherShips)
	{
		otherShip.update(dt);
	}
	ship.update(dt);

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
	: scenes(scenes), client(client), otherClients(clients), ship(Ship(client->getName()))
{
}


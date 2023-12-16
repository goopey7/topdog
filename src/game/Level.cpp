// Sam Collier 2023

#include "Level.h"
#include <thread>

void Level::init()
{
	ship.init(true, client->getName());
	for (const Client& c : *otherClients)
	{
		Ship otherShip;
		otherShip.init(false, c.getName());
		otherShips.push_back(otherShip);
	}
	updateServerThread = new std::thread(&Level::updateServer, this);
	updateClientThread = new std::thread(&Level::updateClient, this);
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
	// draw ship position
	DrawText(std::to_string(ship.getPosition().x).c_str(), 0, 0, 20, RED);
	DrawText(std::to_string(ship.getPosition().y).c_str(), 0, 20, 20, RED);

	for (Ship& otherShip : otherShips)
	{
		otherShip.draw();
	}
	ship.draw();
}

void Level::transitionToMainMenu() { scenes->pop(); }

Level::Level(std::queue<std::unique_ptr<Scene>>* scenes, Client* client,
			 const std::vector<Client>* clients)
	: scenes(scenes), client(client), otherClients(clients)
{
}

void Level::updateServer()
{
	while (true)
	{
		if (lastPositionSent.x != ship.getPosition().x && lastPositionSent.y != ship.getPosition().y)
		{
			client->sendToServer(UpdatePosition(ship.getPosition().x, ship.getPosition().y));
			lastPositionSent = ship.getPosition();
		}
		//client->sendToServer(UpdateVelocity(ship.getVelocity().x, ship.getVelocity().y));
		//client->sendToServer(UpdateRotation(ship.getRotation()));
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void Level::updateClient()
{
	while (true)
	{
		ServerCommand cmd = client->listenToServer();
		if (std::holds_alternative<ClientUpdatePosition>(cmd))
		{
			// find the ship with the same name as the one in the command
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientUpdatePosition>(cmd).name)
				{
					otherShip.setPosition({std::get<ClientUpdatePosition>(cmd).x,
										   std::get<ClientUpdatePosition>(cmd).y});
					break;
				}
			}
		}
	}
}

Level::~Level()
{
	updateServerThread->detach();
	updateClientThread->detach();
	delete updateServerThread;
	delete updateClientThread;
}

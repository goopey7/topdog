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
	for (int i = 0; i < ship.getBullets().size(); i++)
	{
		for (Ship& otherShip : otherShips)
		{
			if (CheckCollisionCircleRec(ship.getBullets()[i].getPosition(),
										ship.getBullets()[i].getSize(),
										otherShip.getCollisionRect()))
			{
				otherShip.onCollision(ship.getBullets()[i]);
				ship.getBullets().erase(ship.getBullets().begin() + i);
				break;
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
		if ((lastPositionSent.x != ship.getPosition().x &&
			 lastPositionSent.y != ship.getPosition().y) ||
			(lastVelocitySent.x != ship.getVelocity().x &&
			 lastVelocitySent.y != ship.getVelocity().y) ||
			(lastRotationSent != ship.getRotation()))
		{
			auto us = UpdateStatus(ship.getPosition().x, ship.getPosition().y,
											  ship.getVelocity().x, ship.getVelocity().y,
											  ship.getRotation());
			client->sendToServer(us);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void Level::updateClient()
{
	while (true)
	{
		ServerCommand cmd = client->listenToServer();
		if (std::holds_alternative<ClientUpdateStatus>(cmd))
		{
			// find the ship with the same name as the one in the command
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientUpdateStatus>(cmd).name)
				{
					otherShip.setPosition({std::get<ClientUpdateStatus>(cmd).posx,
									  std::get<ClientUpdateStatus>(cmd).posy});
					otherShip.setVelocity({std::get<ClientUpdateStatus>(cmd).velx,
									  std::get<ClientUpdateStatus>(cmd).vely});
					otherShip.setRotation(std::get<ClientUpdateStatus>(cmd).angle);
					break;
				}
			}
		}
		else if (std::holds_alternative<ClientDisconnected>(cmd))
		{
			for (int i = 0; i < otherShips.size(); i++)
			{
				if (otherShips[i].getName() == std::get<ClientDisconnected>(cmd).name)
				{
					otherShips.erase(otherShips.begin() + i);
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

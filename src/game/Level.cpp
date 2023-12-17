// Sam Collier 2023

#include "Level.h"
#include <iostream>
#include <thread>

void Level::init()
{
	ship.init(true, client->getName(), client);
	for (const Client& c : *otherClients)
	{
		Ship otherShip;
		otherShip.init(false, c.getName(), nullptr);
		otherShips.push_back(otherShip);
	}
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
				ship.getBullets().erase(ship.getBullets().begin() + i);
				break;
			}
		}
	}

	for (int i = 0; i < otherShips.size(); i++)
	{
		for (int j = 0; j < otherShips[i].getBullets().size(); j++)
		{
			if (CheckCollisionCircleRec(otherShips[i].getBullets()[j].getPosition(),
										otherShips[i].getBullets()[j].getSize(),
										ship.getCollisionRect()))
			{
				otherShips[i].getBullets().erase(otherShips[i].getBullets().begin() + j);
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

	// send updates to server
	timeSinceLastSend += dt;
	timeSinceLastUpdate += dt;
	updateServer();
	updateClient();

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
	if (timeSinceLastUpdate < clientUpdateRate)
	{
		return;
	}

	if ((lastVelocitySent.x != ship.getVelocity().x &&
		 lastVelocitySent.y != ship.getVelocity().y) ||
		(lastRotationSent != ship.getRotation()))
	{
		auto us = UpdateStatus(ship.getPosition().x, ship.getPosition().y, ship.getVelocity().x,
							   ship.getVelocity().y, ship.getRotation(), false);
		client->sendToServer(us);
		timeSinceLastSend = 0;

		lastVelocitySent = ship.getVelocity();
		lastRotationSent = ship.getRotation();
	}
}

void Level::updateClient()
{
	if (timeSinceLastSend < clientSendRate)
	{
		return;
	}

	std::optional<ServerCommand> cmdOpt = client->listenToServer();

	if (!cmdOpt.has_value())
	{
		return;
	}

	auto cmd = cmdOpt.value();

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
				if (std::get<ClientUpdateStatus>(cmd).fire)
				{
					otherShip.fire();
				}
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
	timeSinceLastUpdate = 0;
}

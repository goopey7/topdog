// Sam Collier 2023

#include "Level.h"
#include <cmath>
#include <iostream>

void Level::init()
{
	ship.init(true, client->getName(), client, nullptr);
	for (const Client& c : *otherClients)
	{
		Ship otherShip;
		otherShip.init(false, c.getName(), nullptr, &clientUpdates);
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
	// send out bullet fires
	if (IsKeyPressed(KEY_SPACE))
	{
		float radians = ship.getRotation() * DEG2RAD;
		Vector2 direction = {sinf(radians), -cosf(radians)};
		auto fire =
			Fire(ship.getPosition().x, ship.getPosition().y, direction.x, direction.y, GetTime());
		client->sendToServer(fire);
	}

	if (timeSinceLastSend < clientSendRate)
	{
		return;
	}

	// positional updates
	if ((lastPositionSent.x != ship.getPosition().x ||
		 lastPositionSent.y != ship.getPosition().y) ||
		(lastVelocitySent.x != ship.getVelocity().x ||
		 lastVelocitySent.y != ship.getVelocity().y) ||
		(lastRotationSent != ship.getRotation()))
	{
		auto us = UpdateStatus(ship.getPosition().x, ship.getPosition().y, ship.getVelocity().x,
							   ship.getVelocity().y, ship.getRotation(),
							   ship.getRotationDirection(), GetTime());
		client->sendToServer(us);
		timeSinceLastSend = 0;

		lastVelocitySent = ship.getVelocity();
		lastRotationSent = ship.getRotation();
	}
}

void Level::updateClient()
{
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
				otherShip.setRotationDirection(std::get<ClientUpdateStatus>(cmd).rotating);
				if (clientUpdates[&otherShip].size() == 3)
				{
					clientUpdates[&otherShip].erase(clientUpdates[&otherShip].begin());
				}
				clientUpdates[&otherShip].push_back(std::get<ClientUpdateStatus>(cmd));
				break;
			}
		}
	}
	else if (std::holds_alternative<ClientFire>(cmd))
	{
		for (Ship& otherShip : otherShips)
		{
			if (otherShip.getName() == std::get<ClientFire>(cmd).name)
			{
				ClientFire fire = std::get<ClientFire>(cmd);
				otherShip.fire(fire.posx, fire.posy, fire.velx, fire.vely, fire.time);
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

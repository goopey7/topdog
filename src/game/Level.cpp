// Sam Collier 2023

#include "Level.h"
#include "Time.h"
#include <cmath>
#include <iostream>

void Level::init()
{
	ship.init(true, client->getName(), client, nullptr, *gameStartTime);
	for (const Client& c : *otherClients)
	{
		Ship otherShip;
		otherShip.init(false, c.getName(), nullptr, &clientUpdates, *gameStartTime);
		otherShips.push_back(otherShip);
	}
}

void Level::update(float dt)
{
	if (gameOver)
	{
		if (IsKeyPressed(KEY_SPACE))
		{
			std::cout << "Exiting game" << std::endl;
			exit(0);
		}
	}
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
				otherShip.onCollision(ship.getBullets()[i]);
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
				ship.onCollision(otherShips[i].getBullets()[j]);
				client->sendToServerTCP(HealthChange(ship.getHealth(), ship.isDead()));
				break;
			}

			for (int k = 0; k < otherShips.size(); k++)
			{
				if (k == i)
				{
					continue;
				}
				if (CheckCollisionCircleRec(otherShips[i].getBullets()[j].getPosition(),
											otherShips[i].getBullets()[j].getSize(),
											otherShips[k].getCollisionRect()))
				{
					otherShips[i].getBullets().erase(otherShips[i].getBullets().begin() + j);
					otherShips[k].onCollision(otherShips[i].getBullets()[j]);
					break;
				}
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
	for (Ship& otherShip : otherShips)
	{
		otherShip.draw();
	}
	ship.draw();

	if (gameOver)
	{
		std::string winMsg = winner + " wins!";
		DrawText(winMsg.c_str(), GetScreenWidth() / 2 - MeasureText(winMsg.c_str(), 50) / 2,
				 GetScreenHeight() / 2 - 50, 50, WHITE);
		DrawText("Press SPACE to exit", GetScreenWidth() / 2 - MeasureText("Press space to exit", 20) / 2,
				 GetScreenHeight() / 2 + 50, 20, WHITE);
	}
	else
	{
		int numDead = 0;
		for (Ship& otherShip : otherShips)
		{
			if (otherShip.isDead())
			{
				numDead++;
			}
		}
		if (numDead == otherShips.size() - 1 && ship.isDead())
		{
			gameOver = true;
			for (Ship& otherShip : otherShips)
			{
				if (!otherShip.isDead())
				{
					winner = otherShip.getName();
					break;
				}
			}
		}
		else if (numDead == otherShips.size() && !ship.isDead())
		{
			gameOver = true;
			winner = client->getName();
		}
	}
}

void Level::transitionToMainMenu() { scenes->pop(); }

Level::Level(std::queue<std::unique_ptr<Scene>>* scenes, Client* client,
			 const std::vector<Client>* clients, long long* gameStartTime)
	: scenes(scenes), client(client), otherClients(clients), gameStartTime(gameStartTime)
{
}

void Level::updateServer()
{
	// send out bullet fires
	if (IsKeyPressed(KEY_SPACE))
	{
		float radians = ship.getRotation() * DEG2RAD;
		Vector2 direction = {sinf(radians), -cosf(radians)};
		auto fire = Fire(ship.getPosition().x, ship.getPosition().y, direction.x, direction.y,
						 getElapsedTimeInSeconds(*gameStartTime));
		client->sendToServerTCP(fire);
	}

	timeSinceLastPositionalUpdate += GetFrameTime();
	timeSinceLastVelocityUpdate += GetFrameTime();
	timeSinceLastRotationalUpdate += GetFrameTime();

	// velocity updates
	if ((lastVelocitySent.x != ship.getVelocity().x ||
		 lastVelocitySent.y != ship.getVelocity().y) &&
		timeSinceLastVelocityUpdate > velocityUpdateRate)
	{
		auto uv = UpdateVel(ship.getVelocity().x, ship.getVelocity().y,
							getElapsedTimeInSeconds(*gameStartTime));
		client->sendToServerUDP(uv);

		lastVelocitySent = ship.getVelocity();
		timeSinceLastVelocityUpdate = 0;
	}

	// positional updates
	if ((lastPositionSent.x != ship.getPosition().x ||
		 lastPositionSent.y != ship.getPosition().y) &&
		timeSinceLastPositionalUpdate > positionalUpdateRate)
	{
		auto up = UpdatePos(ship.getPosition().x, ship.getPosition().y,
							getElapsedTimeInSeconds(*gameStartTime));
		client->sendToServerTCP(up);

		lastPositionSent = ship.getPosition();
		timeSinceLastPositionalUpdate = 0;
	}

	if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_RIGHT) ||
		IsKeyPressed(KEY_D))
	{
		auto ur =
			RotStart(ship.getRotation(), (IsKeyPressed(KEY_LEFT) || (IsKeyPressed(KEY_A))) ? -1 : 1,
					 getElapsedTimeInSeconds(*gameStartTime));
		client->sendToServerTCP(ur);
	}

	if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_A) || IsKeyReleased(KEY_RIGHT) ||
		IsKeyReleased(KEY_D))
	{
		auto ur = RotEnd(ship.getRotation(), getElapsedTimeInSeconds(*gameStartTime));
		client->sendToServerTCP(ur);
	}
}

void Level::updateClient()
{
	std::optional<std::queue<ServerCommand>> cmdQueueOpt = client->listenToServer();

	if (!cmdQueueOpt.has_value())
	{
		return;
	}

	auto cmdQueue = cmdQueueOpt.value();

	while (!cmdQueue.empty())
	{
		ServerCommand cmd = cmdQueue.front();
		cmdQueue.pop();

		if (std::holds_alternative<GameOver>(cmd))
		{
			winner = std::get<GameOver>(cmd).winner;
			gameOver = true;
		}
		else if (std::holds_alternative<ClientUpdateVel>(cmd))
		{
			// find the ship with the same name as the one in the command
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientUpdateVel>(cmd).name)
				{
					otherShip.setVelocity(
						{std::get<ClientUpdateVel>(cmd).velx, std::get<ClientUpdateVel>(cmd).vely});
					if (clientUpdates[&otherShip].size() == 3)
					{
						clientUpdates[&otherShip].erase(clientUpdates[&otherShip].begin());
					}
					clientUpdates[&otherShip].push_back(std::get<ClientUpdateVel>(cmd));
					break;
				}
			}
		}
		else if (std::holds_alternative<ClientHealthChange>(cmd))
		{
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientHealthChange>(cmd).name)
				{
					otherShip.setHealth(std::get<ClientHealthChange>(cmd).health);
					otherShip.setDead(std::get<ClientHealthChange>(cmd).isDead);
					break;
				}
			}
		}
		else if (std::holds_alternative<ClientUpdatePos>(cmd))
		{
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientUpdatePos>(cmd).name)
				{
					auto up = std::get<ClientUpdatePos>(cmd);
					otherShip.setPosition({up.posx, up.posy});
					break;
				}
			}
		}
		else if (std::holds_alternative<ClientRotStart>(cmd))
		{
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientRotStart>(cmd).name)
				{
					auto ur = std::get<ClientRotStart>(cmd);
					otherShip.startRotation(ur.angle, ur.dir, ur.time);
					break;
				}
			}
		}
		else if (std::holds_alternative<ClientRotEnd>(cmd))
		{
			for (Ship& otherShip : otherShips)
			{
				if (otherShip.getName() == std::get<ClientRotEnd>(cmd).name)
				{
					auto ur = std::get<ClientRotEnd>(cmd);
					otherShip.endRotation(ur.angle);
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
	}
}

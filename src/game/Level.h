// Sam Collier 2023
#pragma once

#include "Client.h"
#include "Scene.h"
#include "Ship.h"
#include <memory>
#include <queue>
#include <thread>

class Level : public Scene
{
  public:
	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;
	Level(std::queue<std::unique_ptr<Scene>>* scenes, Client* client,
		  const std::vector<Client>* clients);

	void init() final;
	void update(float dt) final;
	void draw() final;
	void transitionToMainMenu();

	void updateServer();
	void updateClient();

  private:
	Ship ship;
	std::vector<Ship> otherShips;
	std::queue<std::unique_ptr<Scene>>* scenes;
	Client* client;
	const std::vector<Client>* otherClients;

	Vector2 lastVelocitySent = {-1.f, -12.f};
	float lastRotationSent = -1.f;

	float clientUpdateRate = 1.0f / 10.0f;
	float clientSendRate = 1.0f / 10.0f;
	float timeSinceLastUpdate = 0;
	float timeSinceLastSend = 0;
};

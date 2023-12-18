// Sam Collier 2023
#pragma once

#include "Client.h"
#include "Scene.h"
#include "Ship.h"
#include <memory>
#include <queue>
#include <map>

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

	Vector2 lastPositionSent = {0.f, 0.f};
	Vector2 lastVelocitySent = {-1.f, -12.f};
	float lastRotationSent = -1.f;

	float positionalUpdateRate = 0.7f;
	float timeSinceLastPositionalUpdate = 0;

	float velocityUpdateRate = 0.f;
	float timeSinceLastVelocityUpdate = 0;

	float rotationalUpdateRate = 0.7f;
	float timeSinceLastRotationalUpdate = 0;

	std::map<Ship*, std::vector<ClientUpdateVel>> clientUpdates;
};

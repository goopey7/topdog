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
	~Level();

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
	std::thread* updateServerThread;
	std::thread* updateClientThread;

	Vector2 lastVelocitySent = {0, 0};
	float lastRotationSent = 0;
};

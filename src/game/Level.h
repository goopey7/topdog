// Sam Collier 2023
#pragma once

#include "Client.h"
#include "Scene.h"
#include "Ship.h"
#include <memory>
#include <queue>

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

  private:
	Ship ship;
	Ship enemy;
	std::queue<std::unique_ptr<Scene>>* scenes;
	Client* client;
	const std::vector<Client>* clients;
};

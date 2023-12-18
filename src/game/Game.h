// Sam Collier 2023
#pragma once

#include "Scene.h"
#include "Client.h"
#include <memory>
#include <queue>

class Game
{
  public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game() = default;
	~Game();
	void init();
	void update(float dt);
	void draw();
	void listenToServer();
	void shutdown();
  private:
	std::queue<std::unique_ptr<Scene>> scenes;
	void mainMenu();
	void lobbyMenu();
	void nextScene();
	void connect();
	Client client;
	std::vector<Client> otherClients;
	std::string nameInput;
	bool inGame = false;
	long long gameStartTime = 0;
};

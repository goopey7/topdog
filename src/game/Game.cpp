// Sam Collier 2023

#include "Game.h"
#include "Level.h"
#include "Menu.h"
#include <imgui.h>
#include <imgui_stdlib.h>

#include <raylib.h>

void Game::init()
{
	scenes.push(std::make_unique<Menu>([this]() { this->mainMenu(); }));
	scenes.push(std::make_unique<Menu>([this]() { this->lobbyMenu(); }));
	scenes.push(std::make_unique<Level>(&scenes));
	scenes.front()->init();
}

void Game::update(float dt) { scenes.front()->update(dt); }

void Game::draw() { scenes.front()->draw(); }

void Game::nextScene()
{
	scenes.pop();
	scenes.front()->init();
}

void Game::mainMenu()
{
	ImGui::SetNextWindowPos(
		ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
		ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowBgAlpha(0.0f); // make window transparent

	if (ImGui::Begin("Connection", NULL,
					 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
						 ImGuiWindowFlags_NoResize))
	{

		ImGui::PushItemWidth(200.0f);
		ImGui::InputText("Name", &nameInput);
		ImGui::PopItemWidth();

		if (ImGui::Button("Connect"))
		{
			client.init(nameInput);
			client.connectToServer("127.0.0.1", 4916);
			nextScene();
		}
	}
	ImGui::End();
}

void Game::lobbyMenu()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	ImGui::Begin("Lobby", nullptr,
				 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
					 ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Player Name: %s", client.getName().c_str());
	if (ImGui::Button("Toggle Ready"))
	{
		client.toggleReady();
	}

	ImGui::Text("Players");
	ImGui::Separator();

	for (auto& c : otherClients)
	{
		std::string name = c.getName();
		bool isReady = c.isReadyToStart();

		ImGui::Text("%s", name.c_str());
	}

	ImGui::End();

	ImGui::PopStyleVar(2);
}

Game::~Game()
{
	if (client.isConnected())
	{
		client.closeConnection();
	}
}

void Game::listenToServer()
{
	while (true)
	{
		if (client.isConnected())
		{
			std::string serverMsg = client.listenToServer();
			if (serverMsg.find("new_client:") != std::string::npos)
			{
				std::string name = serverMsg.substr(11);
				Client newClient;
				newClient.init(name);
				otherClients.push_back(newClient);
			}
		}
	}
}

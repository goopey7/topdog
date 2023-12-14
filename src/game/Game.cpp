// Sam Collier 2023

#include "Game.h"
#include "Client.h"
#include "Level.h"
#include "Menu.h"
#include <imgui.h>
#include <imgui_stdlib.h>

#include <iostream>
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
			const std::vector<std::string> lobbyInfo = client.connectToServer("127.0.0.1", 4916);
			for (auto& s : lobbyInfo)
			{
				Client newClient;
				newClient.init(s);
				otherClients.push_back(newClient);
				std::cout << "Added client " << s << std::endl;
			}
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

	ImGui::Text("Other Players");
	ImGui::Separator();

	for (auto& c : otherClients)
	{
		std::string name = c.getName();
		bool isReady = c.isReadyToStart();
		if (isReady)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", name.c_str());
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", name.c_str());
		}
	}

	ImGui::End();

	ImGui::PopStyleVar(2);
}

Game::~Game()
{
	shutdown();
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
			else if (serverMsg.find("client_ready:") != std::string::npos)
			{
				std::string name = serverMsg.substr(13);
				for (auto& c : otherClients)
				{
					if (c.getName() == name)
					{
						c.setReady(true);
					}
				}
			}
			else if (serverMsg.find("client_not_ready:") != std::string::npos)
			{
				std::string name = serverMsg.substr(17);
				for (auto& c : otherClients)
				{
					if (c.getName() == name)
					{
						c.setReady(false);
					}
				}
			}
		}
	}
}

void Game::shutdown()
{
	if (client.isConnected())
	{
		client.closeConnection();
	}
}


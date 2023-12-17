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
	scenes.push(std::make_unique<Level>(&scenes, &client, &otherClients));
	scenes.front()->init();
}

void Game::update(float dt)
{
	scenes.front()->update(dt);
	listenToServer();
}

void Game::draw() { scenes.front()->draw(); }

void Game::nextScene()
{
	scenes.pop();
	scenes.front()->init();
}

void Game::connect()
{
	client.init(nameInput);
	otherClients = client.connectToServer("127.0.0.1", 4916);
	for (auto& s : otherClients)
	{
		std::cout << "Added client " << s.getName() << std::endl;
	}
	nextScene();
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
		if (ImGui::GetIO().KeysDown[ImGuiKey_Enter])
		{
			this->connect();
		}
		ImGui::PopItemWidth();

		if (ImGui::Button("Connect"))
		{
			this->connect();
		}
	}
	ImGui::End();
}

void Game::lobbyMenu()
{
	if (inGame)
	{
		nextScene();
		return;
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	ImGui::Begin("Lobby", nullptr,
				 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
					 ImGuiWindowFlags_NoCollapse);

	if (client.isReadyToStart())
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", client.getName().c_str());
	}
	else
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", client.getName().c_str());
	}

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

Game::~Game() { shutdown(); }

void Game::listenToServer()
{
	if (client.isConnected())
	{
		std::optional<ServerCommand> cmdOpt = client.listenToServer();
		if (!cmdOpt.has_value())
		{
			return;
		}

		ServerCommand cmd = cmdOpt.value();
		if (std::holds_alternative<NewClient>(cmd))
		{
			NewClient newClient = std::get<NewClient>(cmd);
			Client c;
			c.init(newClient.name);
			otherClients.push_back(c);
		}
		else if (std::holds_alternative<ClientReady>(cmd))
		{
			ClientReady ready = std::get<ClientReady>(cmd);
			for (auto& c : otherClients)
			{
				if (c.getName() == ready.name)
				{
					c.setReady(ready.ready);
				}
			}
		}
		else if (std::holds_alternative<ClientDisconnected>(cmd))
		{
			ClientDisconnected disconnected = std::get<ClientDisconnected>(cmd);
			for (int i = 0; i < otherClients.size(); i++)
			{
				if (otherClients[i].getName() == disconnected.name)
				{
					otherClients.erase(otherClients.begin() + i);
				}
			}
		}
		else if (std::holds_alternative<StartGame>(cmd))
		{
			std::cout << "Starting game!" << std::endl;
			inGame = true;
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

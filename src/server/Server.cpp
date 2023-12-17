// Sam Collier 2023
#include "Server.h"
#include <iostream>
#include <string>

void Server::startServer(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4916);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	int result = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
		throw std::runtime_error("Can't bind socket to IP/port");
	}

	// Set socket to non-blocking
	int option = 1;
	ioctl(serverSocket, FIONBIO, &option);

	listen(serverSocket, 5);
}

void Server::sendToClients(const ServerCommand& cmd, int indexToSkip)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (i == indexToSkip)
		{
			continue;
		}
		if (std::holds_alternative<StartGame>(cmd))
		{
			std::cout << "Sending start game to client " << i << std::endl;
		}
		clients[i].sendMsg(STRINGIFY_SERVER_COMMAND(cmd));
	}
}

void Server::acceptIncomingClients()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);
	FD_SET(serverSocket, &readSet);
	maxFd = std::max(maxFd, serverSocket);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return;
	}

	if (!FD_ISSET(serverSocket, &readSet))
	{
		return;
	}

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == -1)
	{
		std::cerr << "Can't accept client socket" << std::endl;
		return;
	}

	char clientMsg[1024];
	result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-1: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;

	std::string msg = "Welcome to the server!";
	result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
	if (result == -1)
	{
		std::cerr << "Accpt-2: Can't send message to client " << clients.size() << std::endl;
		return;
	}

	result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-3: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	if (std::string(clientMsg) != msg)
	{
		msg = "Accpt: Erroneous data received from client";
		send(clientSocket, msg.c_str(), msg.size() + 1, 0);
		std::cerr << msg << std::endl;
		return;
	}

	msg = "get_lobby_info:";
	for (int i = 0; i < clients.size(); i++)
	{
		msg += clients[i].getName();
		msg += ":";
		msg += clients[i].isReady() ? "1" : "0";
		if (i != clients.size() - 1)
		{
			msg += ",";
		}
	}
	if (clients.size() > 0)
	{
		msg += ",";
	}
	else
	{
		msg += ";";
	}

	result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
	result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-4: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	msg = "success";
	result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);

	std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;
	std::cout << "Accpt: Client " << clients.size() << " connected!" << std::endl;

	// Set socket to non-blocking
	int option = 1;
	ioctl(clientSocket, FIONBIO, &option);

	clients.emplace_back(clientSocket, clientMsg);
	sendToClients(NewClient(clientMsg), clients.size() - 1);
}

void Server::handleClientLobbyMsgs()
{
	if (clients.empty())
	{
		return;
	}

	if (!clientsAreReady() || clients.size() < 2)
	{
		receiveAndHandleMsgs();
	}
	else
	{
		sendToClients(StartGame());
		bGameStarted = true;
	}
}

void Server::runGame() { receiveAndHandleMsgs(); }

bool Server::clientsAreReady() const
{
	for (const Client& client : clients)
	{
		if (!client.isReady())
		{
			return false;
		}
	}
	return true;
}

void Server::processMsg(const ClientCommand msg, int index)
{
	std::string name = clients[index].getName();
	if (std::holds_alternative<Disconnect>(msg))
	{
		std::cout << "Client " << index << " disconnected!" << std::endl;
		sendToClients(ClientDisconnected(name), index);
		clients.erase(clients.begin() + index);
	}
	else if (std::holds_alternative<Ready>(msg))
	{
		Ready readyCmd = std::get<Ready>(msg);
		clients[index].setReady(readyCmd.ready);
		if (!clientsAreReady())
		{
			sendToClients(ClientReady(name, readyCmd.ready), index);
		}
	}
	else if (std::holds_alternative<UpdateStatus>(msg))
	{
		UpdateStatus us = std::get<UpdateStatus>(msg);
		sendToClients(ClientUpdateStatus(name, us.posx, us.posy, us.velx, us.vely, us.angle,
										 us.fire, us.rotating, us.time),
					  index);
	}
}

void Server::receiveAndHandleMsgs()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);

	for (int i = 0; i < clients.size(); i++)
	{
		int clientSocket = clients[i].getSocket();
		FD_SET(clientSocket, &readSet);
		maxFd = std::max(maxFd, clientSocket);
	}

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return;
	}

	for (int i = 0; i < clients.size(); i++)
	{
		int clientSocket = clients[i].getSocket();
		if (FD_ISSET(clientSocket, &readSet))
		{
			int bytesAvailable;
			ioctl(clientSocket, FIONREAD, &bytesAvailable);

			// would block
			if (bytesAvailable <= 0)
			{
				continue;
			}

			char clientMsg[1024];
			result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
			if (result == -1)
			{
				std::cerr << "Can't receive message from client" << std::endl;
				continue;
			}
			std::cout << "Client " << i << ": " << clientMsg << std::endl;
			auto cmd = parseClientCommand(clientMsg);
			processMsg(cmd, i);
		}
	}
}

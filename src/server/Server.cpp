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

	listen(serverSocket, 5);
}

void Server::sendToClients(const std::string& message, int indexToSkip)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (i == indexToSkip)
		{
			continue;
		}
		clients[i].sendMsg(message);
	}
}

void Server::acceptIncomingClients()
{
	while (true)
	{
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

		char clientMsg[1024];
		int result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
		if (result == -1)
		{
			std::cerr << "Accpt: Can't receive message from client " << clients.size() << std::endl;
			continue;
		}

		std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;

		std::string msg = "Welcome to the server!";
		result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
		if (result == -1)
		{
			std::cerr << "Accpt: Can't send message to client " << clients.size() << std::endl;
			continue;
		}

		result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
		if (result == -1)
		{
			std::cerr << "Accpt: Can't receive message from client " << clients.size() << std::endl;
			continue;
		}

		if (std::string(clientMsg) != msg)
		{
			msg = "Accpt: Erroneous data received from client";
			send(clientSocket, msg.c_str(), msg.size() + 1, 0);
			std::cerr << msg << std::endl;
			continue;
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
			std::cerr << "Accpt: Can't receive message from client " << clients.size() << std::endl;
			continue;
		}

		msg = "success";
		result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);

		// Set socket to non-blocking
		int option = 1;
		ioctl(clientSocket, FIONBIO, &option);

		std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;
		std::cout << "Accpt: Client " << clients.size() << " connected!" << std::endl;
		clients.emplace_back(clientSocket, std::string(clientMsg));
		sendToClients("new_client:" + std::string(clientMsg), clients.size() - 1);
	}
}

void Server::handleClientLobbyMsgs()
{
	while (!clientsAreReady() || clients.size() < 2)
	{
		receiveAndHandleMsgs();
	}
	sendToClients("start_game");
}

void Server::runGame()
{
	while (true)
	{
		receiveAndHandleMsgs();
	}
}

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
	if (std::holds_alternative<Disconnect>(msg))
	{
		std::cout << "Client " << index << " disconnected!" << std::endl;
		sendToClients("client_disconnected:" + clients[index].getName(), index);
		clients.erase(clients.begin() + index);
	}
	else if (std::holds_alternative<Ready>(msg))
	{
		Ready readyCmd = std::get<Ready>(msg);
		clients[index].setReady(readyCmd.ready);
		if (readyCmd.ready)
		{
			std::cout << "Client " << index << " is ready!" << std::endl;
			sendToClients("client_ready:" + clients[index].getName(), index);
		}
		else
		{
			std::cout << "Client " << index << " is not ready!" << std::endl;
			sendToClients("client_not_ready:" + clients[index].getName(), index);
		}
	}
	else if (std::holds_alternative<UpdatePosition>(msg))
	{
		UpdatePosition pos = std::get<UpdatePosition>(msg);
		std::cout << "Client " << index << " updated position to (" << pos.x << ", " << pos.y
				  << ")\n";
	}
}

void Server::receiveAndHandleMsgs()
{
	fd_set readSet;
	int maxFd = 0;

	if (clients.empty())
	{
		return;
	}

	FD_ZERO(&readSet);

	for (int i = 0; i < clients.size(); i++)
	{
		int clientSocket = clients[i].getSocket();
		FD_SET(clientSocket, &readSet);
		maxFd = std::max(maxFd, clientSocket);
	}

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
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
			std::cout << "Client " << i << " sent command " << std::string(clientMsg) << std::endl;
			auto cmd = parseCommand(clientMsg);
			processMsg(cmd, i);
		}
	}
}

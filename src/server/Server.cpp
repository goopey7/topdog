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

void Server::sendToClients(const std::string& message)
{
	for (const Client& client : clients)
	{
		send(client.getSocket(), message.c_str(), message.size() + 1, 0);
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

		std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;
		std::cout << "Accpt: Client " << clients.size() << " connected!" << std::endl;
		msg = "Connection successful";
		result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);

		// Set socket to non-blocking
		int option = 1;
		ioctl(clientSocket, FIONBIO, &option);

		clients.emplace_back(clientSocket);
	}
}

void Server::handleClientMsgs()
{
	// TODO - surely there's a more elegant way to wait for clients
	while (clients.empty())
	{
	}

	fd_set readSet;
	int maxFd = 0;

	while (true)
	{
		FD_ZERO(&readSet);

		for (int i = 0; i < clients.size(); i++)
		{
			int clientSocket = clients[i].getSocket();
			FD_SET(clientSocket, &readSet);
			maxFd = std::max(maxFd, clientSocket);
		}

		int result = select(maxFd + 1, &readSet, NULL, NULL, NULL);
		if (result == -1)
		{
			std::cerr << "Can't select socket" << std::endl;
			continue;
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
				processMsg(clientMsg, i);
			}
		}
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

void Server::processMsg(const char* msg, int index)
{
	switch (msgMap.at(msg))
	{
	case ClientCommand::DISCONNECT:
		std::cout << "Client " << index << " disconnected!" << std::endl;
		clients.erase(clients.begin() + index);
		break;
	case ClientCommand::READY:
		std::cout << "Client " << index << " is ready!" << std::endl;
		clients[index].setReady(true);
		break;
	case ClientCommand::NOT_READY:
		std::cout << "Client " << index << " is not ready!" << std::endl;
		clients[index].setReady(false);
		break;
	default:
		break;
	}
}

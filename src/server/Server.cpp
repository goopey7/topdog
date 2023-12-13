// Sam Collier 2023

#include "Server.h"
#include <iostream>

void Server::startServer(int port)
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

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
	for (int client : clients)
	{
		send(client, message.c_str(), message.size() + 1, 0);
	}
}

void Server::acceptIncomingClient()
{
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

	char clientMsg[1024];
	int result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from client" << std::endl;
		return;
	}

	std::cout << "Client: " << clientMsg << std::endl;

	std::string msg = "Welcome to the server!";
	result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
	if (result == -1)
	{
		std::cerr << "Can't send message to client" << std::endl;
		return;
	}

	result = recv(clientSocket, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from client" << std::endl;
		return;
	}

	if (std::string(clientMsg) != msg)
	{
		msg = "Erroneous data received from client";
		send(clientSocket, msg.c_str(), msg.size() + 1, 0);
		std::cerr << msg << std::endl;
		return;
	}

	std::cout << "Client: " << clientMsg << std::endl;
	std::cout << "Client connected!" << std::endl;
	msg = "Connection successful!";
	result = send(clientSocket, msg.c_str(), msg.size() + 1, 0);

	clients.push_back(clientSocket);
}

void Server::listenForClientMsg()
{
	char clientMsg[1024];
	int result = recv(clients[0], clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from client" << std::endl;
		return;
	}

	if (std::string(clientMsg) == "disconnect")
	{
		std::cout << "Client disconnected!" << std::endl;
		return;
	}
	std::cout << "Client: " << clientMsg << std::endl;
}


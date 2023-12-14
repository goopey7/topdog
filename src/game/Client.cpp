// Sam Collier 2023

#include "Client.h"
#include <iostream>

/*
# Connects to a server at the given ip and port
- The connection procedure is:
- send client info to server
- server replies with a message
- client must send that message back
- if server is happy then client will receive a welcome message
*/
void Client::connectToServer(const std::string& ip, int port)
{
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	int result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
		std::cerr << "Can't connect to server";
		return;
	}

	sendToServer("connect");
	char serverMsg[1024];
	result = recv(clientSocket, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return;
	}
	sendToServer(serverMsg);

	result = recv(clientSocket, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return;
	}

	std::cout << "Server: " << serverMsg << std::endl;
	isConnectedToServer = true;
}

void Client::sendToServer(const std::string& message)
{
	send(clientSocket, message.c_str(), message.size() + 1, 0);
}

void Client::closeConnection()
{
	sendToServer("disconnect");
	close(clientSocket);
	isConnectedToServer = false;
}

bool Client::isConnected() const
{
	return isConnectedToServer;
}

void Client::toggleReady()
{
	isReady = !isReady;
	sendToServer(isReady ? "ready" : "not_ready");
}


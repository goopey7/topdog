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
const std::vector<Client> Client::connectToServer(const std::string& ip, int port)
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
		return {};
	}

	sendToServer("connect");
	char serverMsg[1024];
	result = recv(clientSocket, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return {};
	}
	sendToServer(serverMsg);

	result = recv(clientSocket, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return {};
	}

	std::cout << "Server: " << serverMsg << std::endl;
	if (std::string(serverMsg).find("get_lobby_info:") != std::string::npos)
	{
		sendToServer(name);
		std::string serverMsgStr = serverMsg;
		std::vector<std::string> lobbyInfo;

		// split the message into a vector by commas
		if (serverMsgStr.back() == ',')
		{
			std::string delimiter = ",";
			size_t pos = 0;
			std::string token;
			serverMsgStr.erase(0, 15); // remove "get_lobby_info:"
			while ((pos = serverMsgStr.find(delimiter)) != std::string::npos)
			{
				token = serverMsgStr.substr(0, pos);
				lobbyInfo.push_back(token);
				serverMsgStr.erase(0, pos + delimiter.length());
			}
		}
		else
		{
			lobbyInfo = {};
		}

		result = recv(clientSocket, serverMsg, sizeof(serverMsg), 0);
		if (result == -1)
		{
			std::cerr << "Can't receive message from server";
			return {};
		}
		std::cout << "Server: " << serverMsg << std::endl;
		isConnectedToServer = true;

		// Set socket to non-blocking
		int option = 1;
		ioctl(clientSocket, FIONBIO, &option);

		std::vector<Client> clients;
		for (int i = 0; i < lobbyInfo.size(); i++)
		{
			Client client;
			std::string name = lobbyInfo[i].substr(0, lobbyInfo[i].find(":"));
			bool ready = lobbyInfo[i].substr(lobbyInfo[i].find(":") + 1) == "1";
			client.init(name);
			client.setReady(ready);
			clients.push_back(client);
		}
		return clients;
	}
	else
	{
		std::cerr << "Server Unhappy" << std::endl;
	}
	return {};
}

void Client::sendToServer(const ClientCommand& command)
{
	std::string message = STRINGIFY_CLIENT_COMMAND(command);
	if (std::holds_alternative<UpdatePos>(command))
	{
		std::cout << "Client " << name << ": " << message << std::endl;
	}
	send(clientSocket, message.c_str(), message.size() + 1, 0);
}

void Client::sendToServer(const std::string& message)
{
	send(clientSocket, message.c_str(), message.size() + 1, 0);
}

void Client::closeConnection()
{
	sendToServer(Disconnect());
	close(clientSocket);
	isConnectedToServer = false;
}

bool Client::isConnected() const { return isConnectedToServer; }

void Client::toggleReady()
{
	isReady = !isReady;
	sendToServer(Ready(isReady));
}

std::optional<ServerCommand> Client::listenToServer()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);
	FD_SET(clientSocket, &readSet);
	maxFd = std::max(maxFd, clientSocket);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return std::nullopt;
	}

	if (!FD_ISSET(clientSocket, &readSet))
	{
		return std::nullopt;
	}

	std::string serverMsg;
	char msg[1024];
	result = recv(clientSocket, msg, sizeof(msg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
	}
	return parseServerCommand(msg);
}

void Client::init(const std::string& name) { this->name = name; }

const std::string& Client::getName() const { return name; }

bool Client::isReadyToStart() const { return isReady; }

void Client::setReady(bool ready) { this->isReady = ready; }

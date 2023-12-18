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

- returns a vector of clients in the lobby
*/
const std::vector<Client> Client::connectToServer(const std::string& ip)
{
	socketUDP = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in udpBindAddr;
	udpBindAddr.sin_family = AF_INET;
	udpBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpBindAddr.sin_port = htons(0);

	int result = bind(socketUDP, (struct sockaddr*)&udpBindAddr, sizeof(udpBindAddr));
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	getsockname(socketUDP, (struct sockaddr*)&addr, &addrLen);

	// Set socket to non-blocking
	int option = 1;
	ioctl(socketUDP, FIONBIO, &option);

	serverIP = ip;
	socketTCP = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_TCP_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	result = connect(socketTCP, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
		std::cerr << "Can't connect to server";
		return {};
	}

	sendToServerTCP("connect");
	char serverMsg[1024];
	result = recv(socketTCP, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return {};
	}
	sendToServerTCP(serverMsg);

	result = recv(socketTCP, serverMsg, sizeof(serverMsg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
		return {};
	}

	std::cout << "Server: " << serverMsg << std::endl;
	if (std::string(serverMsg).find("get_lobby_info:") != std::string::npos)
	{
		sendToServerTCP(name+":"+std::to_string(htons(addr.sin_port)));
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

		result = recv(socketTCP, serverMsg, sizeof(serverMsg), 0);
		if (result == -1)
		{
			std::cerr << "Can't receive message from server";
			return {};
		}
		std::cout << "Server: " << serverMsg << std::endl;

		// Set socket to non-blocking
		ioctl(socketTCP, FIONBIO, &option);

		// initialize other clients
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
		isConnectedToServer = true;
		return clients;
	}
	else
	{
		std::cerr << "Server Unhappy" << std::endl;
	}
	return {};
}

void Client::sendToServerTCP(const ClientCommand& command)
{
	std::string message = STRINGIFY_CLIENT_COMMAND(command);
	send(socketTCP, message.c_str(), message.size() + 1, 0);
}

void Client::sendToServerTCP(const std::string& message)
{
	send(socketTCP, message.c_str(), message.size() + 1, 0);
}

void Client::sendToServerUDP(const ClientCommand& command)
{
	std::string message = STRINGIFY_CLIENT_COMMAND(command);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_UDP_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

	sendto(socketUDP, message.c_str(), message.size() + 1, 0, (sockaddr*)&serverAddr,
		   sizeof(serverAddr));
}

void Client::sendToServerUDP(const std::string& message)
{
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_UDP_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

	sendto(socketUDP, message.c_str(), message.size() + 1, 0, (sockaddr*)&serverAddr,
		   sizeof(serverAddr));
}

void Client::closeConnection()
{
	sendToServerTCP(Disconnect());
	close(socketTCP);
	isConnectedToServer = false;
}

bool Client::isConnected() const { return isConnectedToServer; }

void Client::toggleReady()
{
	isReady = !isReady;
	sendToServerTCP(Ready(isReady));
}

std::optional<std::queue<ServerCommand>> Client::listenToServer(int protocols)
{
	if (!isConnectedToServer)
	{
		return std::nullopt;
	}

	auto udpCommand = listenUDP();
	auto tcpCommand = listenTCP();

	if (!udpCommand.has_value() && !tcpCommand.has_value())
	{
		return std::nullopt;
	}

	std::queue<ServerCommand> commands;

	if (udpCommand.has_value())
	{
		commands.push(udpCommand.value());
	}

	if (tcpCommand.has_value())
	{
		commands.push(tcpCommand.value());
	}

	return commands;
}

void Client::init(const std::string& name) { this->name = name; }

const std::string& Client::getName() const { return name; }

bool Client::isReadyToStart() const { return isReady; }

void Client::setReady(bool ready) { this->isReady = ready; }

std::optional<ServerCommand> Client::listenTCP()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);
	FD_SET(socketTCP, &readSet);
	maxFd = std::max(maxFd, socketTCP);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return std::nullopt;
	}

	if (!FD_ISSET(socketTCP, &readSet))
	{
		return std::nullopt;
	}

	char msg[1024];
	result = recv(socketTCP, msg, sizeof(msg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
	}

	return parseServerCommand(msg);
}

std::optional<ServerCommand> Client::listenUDP()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);
	FD_SET(socketUDP, &readSet);
	maxFd = std::max(maxFd, socketUDP);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return std::nullopt;
	}

	if (!FD_ISSET(socketUDP, &readSet))
	{
		return std::nullopt;
	}

	char msg[1024];
	result = recv(socketUDP, msg, sizeof(msg), 0);
	if (result == -1)
	{
		std::cerr << "Can't receive message from server";
	}

	std::string msgStr = msg;
	return parseServerCommand(msg);
}

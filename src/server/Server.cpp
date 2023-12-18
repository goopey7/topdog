// Sam Collier 2023
#include "Server.h"
#include <iostream>
#include <string>

void Server::startServer(int port)
{
	serverTCPSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverUDPSocket = socket(AF_INET, SOCK_DGRAM, 0);

	int opt = 1;
	setsockopt(serverTCPSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(serverUDPSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4916);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	int result = bind(serverTCPSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
		throw std::runtime_error("Can't bind tcp socket to IP/port");
	}

	serverAddr.sin_port = htons(4917);
	result = bind(serverUDPSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == -1)
	{
		throw std::runtime_error("Can't bind udp socket to IP/port");
	}

	// Set socket to non-blocking
	int option = 1;
	ioctl(serverTCPSocket, FIONBIO, &option);
	ioctl(serverUDPSocket, FIONBIO, &option);

	listen(serverTCPSocket, 5);
	listen(serverUDPSocket, 5);
}

void Server::sendToClientsTCP(const ServerCommand& cmd, int indexToSkip)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (i == indexToSkip)
		{
			continue;
		}
		clients[i].sendTCPMsg(STRINGIFY_SERVER_COMMAND(cmd));
	}
}

void Server::sendToClientsUDP(const ServerCommand& cmd, int indexToSkip)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (i == indexToSkip)
		{
			continue;
		}
		clients[i].sendUDPMsg(STRINGIFY_SERVER_COMMAND(cmd));
	}
}

void Server::acceptIncomingClients()
{
	acceptTCP();
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
		sendToClientsTCP(StartGame());
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

void Server::processMsg(const ClientCommand msg, int index, const std::string& debug)
{
	std::string name = clients[index].getName();
	if (std::holds_alternative<Disconnect>(msg))
	{
		std::cout << "Client " << index << " disconnected!" << std::endl;
		sendToClientsTCP(ClientDisconnected(name), index);
		clients.erase(clients.begin() + index);
	}
	else if (std::holds_alternative<Ready>(msg))
	{
		Ready readyCmd = std::get<Ready>(msg);
		clients[index].setReady(readyCmd.ready);
		if (!clientsAreReady())
		{
			sendToClientsTCP(ClientReady(name, readyCmd.ready), index);
		}
	}
	else if (std::holds_alternative<UpdateVel>(msg))
	{
		UpdateVel uv = std::get<UpdateVel>(msg);
		sendToClientsUDP(ClientUpdateVel(name, uv.velx, uv.vely, uv.time), index);
	}
	else if (std::holds_alternative<UpdatePos>(msg))
	{
		UpdatePos up = std::get<UpdatePos>(msg);
		sendToClientsTCP(ClientUpdatePos(name, up.posx, up.posy, up.time), index);
	}
	else if (std::holds_alternative<RotStart>(msg))
	{
		RotStart ur = std::get<RotStart>(msg);
		sendToClientsTCP(ClientRotStart(name, ur.angle, ur.dir, ur.time), index);
	}
	else if (std::holds_alternative<RotEnd>(msg))
	{
		RotEnd ur = std::get<RotEnd>(msg);
		sendToClientsTCP(ClientRotEnd(name, ur.angle, ur.time), index);
	}
	else if (std::holds_alternative<Fire>(msg))
	{
		Fire fire = std::get<Fire>(msg);
		sendToClientsTCP(ClientFire(name, fire.posx, fire.posy, fire.velx, fire.vely, fire.time),
						 index);
	}
}

void Server::receiveAndHandleMsgs()
{
	receiveUDP();
	receiveTCP();
}

void Server::acceptTCP()
{
	fd_set readSetTcp;
	int maxFdTcp = 0;

	FD_ZERO(&readSetTcp);
	FD_SET(serverTCPSocket, &readSetTcp);
	maxFdTcp = std::max(maxFdTcp, serverTCPSocket);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFdTcp + 1, &readSetTcp, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return;
	}

	if (!FD_ISSET(serverTCPSocket, &readSetTcp))
	{
		return;
	}

	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocketTcp = accept(serverTCPSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocketTcp == -1)
	{
		std::cerr << "TCP: Can't accept client socket" << std::endl;
		return;
	}

	char clientMsg[1024];
	result = recv(clientSocketTcp, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-1: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;

	std::string msg = "Welcome to the server!";
	result = send(clientSocketTcp, msg.c_str(), msg.size() + 1, 0);
	if (result == -1)
	{
		std::cerr << "Accpt-2: Can't send message to client " << clients.size() << std::endl;
		return;
	}

	result = recv(clientSocketTcp, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-3: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	if (std::string(clientMsg) != msg)
	{
		msg = "Accpt: Erroneous data received from client";
		send(clientSocketTcp, msg.c_str(), msg.size() + 1, 0);
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

	result = send(clientSocketTcp, msg.c_str(), msg.size() + 1, 0);
	result = recv(clientSocketTcp, clientMsg, sizeof(clientMsg), 0);
	if (result == -1)
	{
		std::cerr << "Accpt-4: Can't receive message from client " << clients.size() << std::endl;
		return;
	}

	msg = "success";
	result = send(clientSocketTcp, msg.c_str(), msg.size() + 1, 0);

	std::cout << "Accpt: Client " << clients.size() << ": " << clientMsg << std::endl;
	std::string clientName = std::string(clientMsg).substr(0, std::string(clientMsg).find(":"));
	int clientPortUDP = std::stoi(std::string(clientMsg).substr(std::string(clientMsg).find(":") + 1));
	std::cout << "Accpt: Client " << clients.size() << " connected!" << std::endl;

	// Set socket to non-blocking
	int option = 1;
	ioctl(clientSocketTcp, FIONBIO, &option);

	// otherwise make a new client
	clients.emplace_back(clientName);
	clients.back().initTCP(clientSocketTcp);
	char clientIP[256];
	inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
	clients.back().initUDP(serverUDPSocket, clientIP, clientPortUDP);
	sendToClientsTCP(NewClient(clientName), clients.size() - 1);
}

void Server::receiveTCP()
{
	fd_set readSet;
	int maxFd = 0;

	FD_ZERO(&readSet);

	for (int i = 0; i < clients.size(); i++)
	{
		int clientSocketTCP = clients[i].getTCPSocket();
		FD_SET(clientSocketTCP, &readSet);
		maxFd = std::max(maxFd, clientSocketTCP);
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
		int clientSocket = clients[i].getTCPSocket();
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
			auto cmds = parseClientCommands(clientMsg);
			for (const auto& cmd : cmds)
			{
				processMsg(cmd, i, clientMsg);
			}
		}
	}
}

void Server::receiveUDP()
{
	fd_set readSet;
	int maxFd = serverUDPSocket;

	FD_ZERO(&readSet);
	FD_SET(serverUDPSocket, &readSet);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int result = select(maxFd + 1, &readSet, NULL, NULL, &timeout);
	if (result == -1)
	{
		std::cerr << "Can't select socket" << std::endl;
		return;
	}

	if (!FD_ISSET(serverUDPSocket, &readSet))
	{
		return;
	}

	char clientMsg[1024];
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	int bytes = recvfrom(serverUDPSocket, clientMsg, sizeof(clientMsg), 0, (sockaddr*)&clientAddr,
						 &clientAddrLen);

	if (bytes > 0)
	{
		// identify client from socket addr
		for (int i = 0; i < clients.size(); i++)
		{
			sockaddr_in addr = clients[i].getUDPAddr();
			if (addr.sin_addr.s_addr == clientAddr.sin_addr.s_addr &&
				addr.sin_port == clientAddr.sin_port)
			{
				auto cmds = parseClientCommands(clientMsg);
				for (const auto& cmd : cmds)
				{
					processMsg(cmd, i, clientMsg);
				}
				return;
			}
		}
	}
}

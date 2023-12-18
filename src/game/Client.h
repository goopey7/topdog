// Sam Collier 2023
#pragma once

#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <queue>
#include <optional>

#include "ClientCommands.h"
#include "ServerCommands.h"

enum ProtocolFlag
{
	TCP = 1 << 0,
	UDP = 1 << 1
};

class Client
{
  public:
	void init(const std::string& name);
	const std::vector<Client> connectToServer(const std::string& ip);
	void sendToServerTCP(const ClientCommand& command);
	void sendToServerTCP(const std::string& message);
	void sendToServerUDP(const ClientCommand& command);
	void sendToServerUDP(const std::string& message);
	const std::string& getName() const;
	std::optional<std::queue<ServerCommand>> listenToServer(int protocol = TCP | UDP);
	void toggleReady();
	void setReady(bool ready);
	void closeConnection();
	bool isConnected() const;
	bool isReadyToStart() const;
  private:
	std::optional<ServerCommand> listenTCP();
	std::optional<ServerCommand> listenUDP();
	int socketTCP;
	int socketUDP;
	bool isReady = false;
	bool isConnectedToServer = false;
	std::string name;
	std::string serverIP;
};

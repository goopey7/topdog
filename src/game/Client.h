// Sam Collier 2023
#pragma once

#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#include <string>
#include <vector>

#include "ClientCommands.h"

class Client
{
  public:
	void init(const std::string& name);
	const std::vector<Client> connectToServer(const std::string& ip, int port);
	void sendToServer(const std::string& message);
	const std::string& getName() const;
	std::string listenToServer();
	void toggleReady();
	void setReady(bool ready);
	void closeConnection();
	bool isConnected() const;
	bool isReadyToStart() const;
  private:
	int clientSocket;
	bool isReady = false;
	bool isConnectedToServer = false;
	std::string name;
};

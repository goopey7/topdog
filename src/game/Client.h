// Sam Collier 2023
#pragma once

#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#include <string>

class Client
{
  public:
	void connectToServer(const std::string& ip, int port);
	void sendToServer(const std::string& message);
	void toggleReady() { isReady = !isReady; }
	void closeConnection();
	bool isConnected() const;
  private:
	int clientSocket;
	bool isReady = false;
	bool isConnectedToServer = false;
};

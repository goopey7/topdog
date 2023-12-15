//Sam Collier 2023
#pragma once

#include "Client.h"
#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <map>

enum class ClientCommand
{
	DISCONNECT = 0,
	READY = 1,
	NOT_READY = 2,
};

class Server
{
	public:
		void startServer(int port);
		void acceptIncomingClients();
		void handleClientLobbyMsgs();
		void sendToClients(const std::string& message, int indexToSkip = -1);
		bool clientsAreReady() const;
		bool shouldClose() const { return bShouldClose; }
		void runGame();
	private:
		int serverSocket;
		std::vector<Client> clients;
		void processMsg(const ClientCommand msg, int index);
		void receiveAndHandleMsgs();
		bool bShouldClose = false;
};


//Sam Collier 2023
#pragma once

#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <map>

#include "Client.h"
#include "ClientCommands.h"
#include <ServerCommands.h>

class Server
{
	public:
		void startServer(int port);
		void acceptIncomingClients();
		void handleClientLobbyMsgs();
		void sendToClients(const ServerCommand& cmd, int indexToSkip = -1);
		bool clientsAreReady() const;
		bool shouldClose() const { return bShouldClose; }
		void runGame();
		bool gameStarted() const { return bGameStarted; }
	private:
		int serverSocket;
		std::vector<Client> clients;
		void processMsg(const ClientCommand msg, int index, const std::string& debug = "");
		void receiveAndHandleMsgs();
		bool bShouldClose = false;
		bool bGameStarted = false;
};


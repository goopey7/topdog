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
	DISCONNECT,
	READY,
	NOT_READY,
};

class Server
{
	public:
		void startServer(int port);
		void acceptIncomingClients();
		void handleClientMsgs();
		void sendToClients(const std::string& message, int indexToSkip);
		bool clientsAreReady() const;
	private:
		int serverSocket;
		std::vector<Client> clients;
		void processMsg(const char* msg, int index);
		std::map<std::string, ClientCommand> msgMap =
		{
			{"DISCONNECT", ClientCommand::DISCONNECT},
			{"disconnect", ClientCommand::DISCONNECT},
			{"READY", ClientCommand::READY},
			{"ready", ClientCommand::READY},
			{"NOT_READY", ClientCommand::NOT_READY},
			{"not_ready", ClientCommand::NOT_READY},
		};
};


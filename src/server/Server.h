//Sam Collier 2023
#pragma once

#ifndef WIN32_
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#include <string>
#include <vector>

class Server
{
	public:
		void startServer(int port);
		void acceptIncomingClient();
		void listenForClientMsg();
		void sendToClients(const std::string& message);
	private:
		int serverSocket;
		std::vector<int> clients;
};


#include "Server.h"
#include <thread>

int main()
{
	Server server;
	server.startServer(4916);
	std::thread serverThread(&Server::handleClientMsgs, &server);
	server.acceptIncomingClients();
	serverThread.join();
}

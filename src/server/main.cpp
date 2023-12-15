#include "Server.h"
#include <thread>

int main()
{
	Server server;
	server.startServer(4916);
	while (!server.shouldClose())
	{
		std::thread acceptIncomingClients(&Server::acceptIncomingClients, &server);
		server.handleClientLobbyMsgs();
		acceptIncomingClients.detach();
		server.runGame();
	}
}

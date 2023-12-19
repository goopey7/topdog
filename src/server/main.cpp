#include "Server.h"
#include <thread>

int main()
{
	Server server;
	server.startServer(4916);
	while (!server.shouldClose())
	{
		if (!server.gameStarted())
		{
			server.acceptIncomingClients();
			server.handleClientLobbyMsgs();
		}
		else
		{
			server.runGame();
		}
	}
	std::cout << "Server shutting down..." << std::endl;
}

#include "Server.h"

int main()
{
	Server server;
	server.startServer(4916);
	server.acceptIncomingClient();
	server.listenForClientMsg();
}

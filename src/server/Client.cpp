// Sam Collier 2023

#include "Client.h"

Client::Client(int socket) : socket(socket) {}

int Client::getSocket() const
{
	return socket;
}

bool Client::isReady() const
{
	return bIsReady;
}

void Client::setReady(bool ready)
{
	bIsReady = ready;
}


// Sam Collier 2023

#include "Client.h"

Client::Client(int socket, const std::string& name) : socket(socket), name(name) {}

int Client::getSocket() const { return socket; }

bool Client::isReady() const { return bIsReady; }

void Client::setReady(bool ready) { bIsReady = ready; }

const std::string& Client::getName() const { return name; }

void Client::sendMsg(const std::string& msg) const
{
	send(socket, msg.c_str(), msg.size() + 1, 0);
}


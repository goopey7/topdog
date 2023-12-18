// Sam Collier 2023

#include "Client.h"

Client::Client(const std::string& name) : name(name) {}

int Client::getTCPSocket() const { return tcpSocket; }

bool Client::isReady() const { return bIsReady; }

void Client::setReady(bool ready) { bIsReady = ready; }

const std::string& Client::getName() const { return name; }

void Client::sendTCPMsg(const std::string& msg) const
{
	send(tcpSocket, msg.c_str(), msg.size() + 1, 0);
}

void Client::sendUDPMsg(const std::string& msg) const
{
	sendto(udpSocket, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&udpAddr, udpAddrLen);
}

int Client::getUDPSocket() const { return udpSocket; }

void Client::initTCP(int socket) { tcpSocket = socket; }

void Client::initUDP(int socket, const std::string& ip, int port)
{
	udpSocket = socket;
	udpAddr.sin_family = AF_INET;
	udpAddr.sin_port = htons(port);
	udpAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	udpAddrLen = sizeof(udpAddr);
}

sockaddr_in Client::getUDPAddr() const { return udpAddr; }

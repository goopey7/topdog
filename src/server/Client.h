// Sam Collier 2023
#pragma once
#include <string>
#ifndef WIN32_
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

class Client
{
  public:
	Client(const std::string& name);
	void initTCP(int socket);
	void initUDP(int socket, const std::string& ip, int port);
	void sendTCPMsg(const std::string& msg) const;
	void sendUDPMsg(const std::string& msg) const;
	int getTCPSocket() const;
	int getUDPSocket() const;
	bool isReady() const;
	void setReady(bool ready);
	const std::string& getName() const;
	sockaddr_in getUDPAddr() const;

	bool getIsDead() const { return isDead; }
	void setIsDead(bool isDead) { this->isDead = isDead; }

  private:
	int tcpSocket;
	int udpSocket;
	sockaddr_in udpAddr;
	socklen_t udpAddrLen;
	bool bIsReady = false;
	std::string name;

	bool isDead = false;
};

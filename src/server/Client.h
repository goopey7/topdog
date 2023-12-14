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
	Client(int socket, const std::string& name);
	void sendMsg(const std::string& msg) const;
	int getSocket() const;
	bool isReady() const;
	void setReady(bool ready);
	const std::string& getName() const;

  private:
	int socket;
	bool bIsReady = false;
	std::string name;
};

// Sam Collier 2023
#pragma once

class Client
{
  public:
	Client(int socket);
	int getSocket() const;
	bool isReady() const;
	void setReady(bool ready);

  private:
	int socket;
	bool bIsReady = false;
};

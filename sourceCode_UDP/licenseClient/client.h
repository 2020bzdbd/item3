#pragma once
#include"public.h"
class client
{
	SOCKET socket1;
	sockaddr_in serverAddr;
	void init();
public:
	client();
	void sendToServer(std::string msg);
	std::string receieveFromServer();
};


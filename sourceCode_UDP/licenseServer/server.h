#pragma once
#include"public.h"

//储存从客户端接收的信息，包含消息和地址
struct messageData {
	std::string msg;
	sockaddr_in addr;
	messageData(std::string, sockaddr_in);
};

//储存客户端信息
struct clientData {
	sockaddr_in clientAddr;
	std::string username;
	std::string password;
	std::string seqNum;
	std::thread* corrThread;
	clientData();
	clientData(sockaddr_in&Addr, std::string name,
		std::string pwd, std::string seqNum, std::thread* thr);
};

//更新队列的元组
struct updateTuple {
	sockaddr_in clientAddr;
	bool statusFlag;
	updateTuple(sockaddr_in addr, bool flag);
};

class server
{
	SOCKET socket1;
	sockaddr_in serverAddr;
	void init();
public:
	server();
	void sendToClient(sockaddr_in* clientAddr, std::string msg);
	void receieveFromClient();
	std::queue<messageData> msgBuffer;		//暂存收到的信息
};


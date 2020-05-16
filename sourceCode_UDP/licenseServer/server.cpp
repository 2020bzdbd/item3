#include "server.h"

server::server()
{
	init();
}

void server::init()
{
	//InitWinsock();
	WSADATA wsaData;
	int port = 1234;						//本机端口
	std::string serverip = "192.168.1.109";	//本机IP

	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //调用Windows Sockets DLL
	{
		printf("Winsock无法初始化!\n");
		WSACleanup();
		exit(-1);
	}

	printf("服务器开始创建SOCKET。\n");
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);				///监听端口
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); ///IP地址设定为本机IP
	socket1 = socket(AF_INET, SOCK_DGRAM, 0);
	bind(socket1, (struct sockaddr*) & serverAddr, sizeof(serverAddr));//将那个结构体绑定到当前的套接字绑定地址以及端口

}

void server::sendToClient(sockaddr_in* clientAddr, std::string msg)
{
	sendto(socket1, msg.c_str(), msg.size(), 0, (struct sockaddr*) clientAddr, sizeof(*clientAddr));
}

void server::receieveFromClient()
{
	printf("开始接收信息...\n");
	while (true) {
		char buffer[128] = "\0";
		sockaddr_in clientAddr;
		int msgLen=sizeof(clientAddr);
		if (recvfrom(socket1, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr,&msgLen) != SOCKET_ERROR) {
			msgBuffer.push(messageData(std::string(buffer), clientAddr));
		}
	}
}

messageData::messageData(std::string m, sockaddr_in a):msg(m),addr(a){}

clientData::clientData()
{
	clientAddr = sockaddr_in();
	username = "";
	password = "";
	seqNum = "";
	corrThread = nullptr;
}

clientData::clientData(sockaddr_in&Addr, std::string name="",
	std::string pwd="", std::string seqNum="",std::thread*thr=nullptr) {
	clientAddr = Addr;
	username = name;
	password = pwd;
	this->seqNum = seqNum;
	corrThread = thr;
};

updateTuple::updateTuple(sockaddr_in addr,bool flag):clientAddr(addr),statusFlag(flag){};



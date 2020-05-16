#include "client.h"

void client::init()
{
	WSADATA wsaData;
	int port = 1234;						//接入端口
	std::string serverIp = "192.168.1.109";//服务器IP

	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //调用Windows Sockets DLL
	{
		printf("Winsock无法初始化!\n");
		WSACleanup();
		exit(-1);
	}

	printf("创建SOCKET。\n");
	serverAddr.sin_family = AF_INET;//UDP的地址簇
	serverAddr.sin_port = htons(port); ///server的监听端口
	serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str()); ///server的地址
	socket1 = socket(AF_INET, SOCK_DGRAM, 0);//数据报格式用于无连接的UDP传输
}

client::client()
{
	init();
}

void client::sendToServer(std::string msg)
{
	sendto(socket1, msg.c_str(), msg.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
}

std::string client::receieveFromServer()
{
	while (true) {
		char buffer[128] = "\0";
		sockaddr_in ServerAddr;
		int msgLen=sizeof(ServerAddr);
		if (recvfrom(socket1, buffer, sizeof(buffer), 0, (struct sockaddr*) & ServerAddr, &msgLen) != SOCKET_ERROR) {
			return std::string(buffer);
		}
	}
}

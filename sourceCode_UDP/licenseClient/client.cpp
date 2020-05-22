#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "client.h"

//将输入的信息根据空格进行分割
vector<string> client::split(char *str)
{
	vector<string> elems;
	const char *delim = " ";
	char *s = strtok(str, delim);
	while (s != NULL)
	{
		elems.push_back(s);
		s = strtok(NULL, delim);
	}
	return elems;
}

void client::init()
{
	WSADATA wsaData;
	int port ;						//接入端口
	std::string serverIp ;//服务器IP
	cout << "按照[ip] [端口号]的格式输入，中间用空格隔开且不加中括号：" << endl;
	string input;
	getline(cin, input, '\n');
	vector<string> info = split((char*)input.c_str());
	serverIp = info[0];
	stringstream ss(info[1]);
	ss >> port;


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
	while (true) 
	{
		char buffer[128] = "\0";
		sockaddr_in ServerAddr;
		int msgLen=sizeof(ServerAddr);
		if (recvfrom(socket1, buffer, sizeof(buffer), 0, (struct sockaddr*) & ServerAddr, &msgLen) != SOCKET_ERROR) {
			return std::string(buffer);
		}
	}
}

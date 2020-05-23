#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "client.h"

//���������Ϣ���ݿո���зָ�
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
	int port ;						//����˿�
	std::string serverIp ;//������IP
	cout << "����[ip] [�˿ں�]�ĸ�ʽ���룬�м��ÿո�����Ҳ��������ţ�" << endl;
	string input;
	getline(cin, input, '\n');
	vector<string> info = split((char*)input.c_str());
	serverIp = info[0];
	stringstream ss(info[1]);
	ss >> port;


	if (WSAStartup(MAKEWORD(2, 1), &wsaData)) //����Windows Sockets DLL
	{
		printf("Winsock�޷���ʼ��!\n");
		WSACleanup();
		exit(-1);
	}

	printf("����SOCKET��\n");
	serverAddr.sin_family = AF_INET;//UDP�ĵ�ַ��
	serverAddr.sin_port = htons(port); ///server�ļ����˿�
	serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str()); ///server�ĵ�ַ
	socket1 = socket(AF_INET, SOCK_DGRAM, 0);//���ݱ���ʽ���������ӵ�UDP����
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
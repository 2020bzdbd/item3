#pragma once
#include"public.h"

//许可证类
class license
{
public:
	string seqNum;
	string username;
	string password;
	int maxNum;//该许可证最大使用人数
	int currentNum;//当前使用该许可证的人数
	bool Is_First_Use;
	vector<string> Users;//当前正在使用的用户的IP+端口号信息

public:
	license()
	{
		seqNum = "";
		username = "";
		password = "";
		maxNum = 0;
		currentNum = 0;
		Is_First_Use = true;
	}
	void readLicenceData(vector<license> &AllLicense);
	void GetFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy);
	void ShowLicenseInfo(vector<license> &AllLicense);
};

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


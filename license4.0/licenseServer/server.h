#pragma once
#include"public.h"

//���֤��
class license
{
public:
	string seqNum;
	string username;
	string password;
	int maxNum;//�����֤���ʹ������
	int currentNum;//��ǰʹ�ø����֤������
	bool Is_First_Use;
	vector<string> Users;//��ǰ����ʹ�õ��û���IP+�˿ں���Ϣ

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

//����ӿͻ��˽��յ���Ϣ��������Ϣ�͵�ַ
struct messageData {
	std::string msg;
	sockaddr_in addr;
	messageData(std::string, sockaddr_in);
};

//����ͻ�����Ϣ
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

//���¶��е�Ԫ��
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
	std::queue<messageData> msgBuffer;		//�ݴ��յ�����Ϣ
};


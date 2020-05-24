//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include"public.h"
#include"server.h"
#include<fstream>
using namespace std;
server myServer;
license myLicense;
std::queue<updateTuple> updateBuffer;	//�ݴ�����еĸ���
std::map<sockaddr_in, clientData> clientInfo;		//ʹ�ÿͻ��˵�ַ���ҿͻ�����Ϣ
vector<license> AllLicense;//�洢��ǰ���������е����֤��Ϣ
vector<string> forbiddenIPs;//�洢�������������IP��ַ�����ɵ�¼
queue<string> cmdQueue;//������У����ļ��ж�ȡ�����ȡ���Ժ����

//���ļ�·���ĺ궨��
#define FILEPATH "license"
#define HistoryFILEPATH "history.txt"
#define ForbidFILEPATH "forbid.txt"
#define CMDFILEPATH "cmd.txt"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

string sourcePath = FILEPATH;

bool operator<(const sockaddr_in& l, const sockaddr_in& r) {
	return l.sin_port < r.sin_port;
}

void connectClient(sockaddr_in clientAddr) {
	//���Ӧ�Ŀͻ��˽���
	//�趨һ����ʱ��һ��ʱ��û�յ�check��ر��߳�
	clock_t time_start = clock();//��ʼ��ʱ
	clock_t time_end;
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == CK_MSG) {
				//���ü�ʱ��
				time_start = clock();
				myServer.sendToClient(&clientAddr, "permit");
			}

			if (msg.message == WM_QUIT) {
				return;
			}

		}
		time_end = clock();
		//���ڵ���30���ӿͻ���û�з���check��Ϣ����Ϊ�ͻ�����������������
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC >= 1800)
		{
			updateBuffer.push(updateTuple(clientAddr, false));
			return;
		}
	}
}


//��ȡ��ǰϵͳʱ��
string GetTime()
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//תΪ�ַ���
	stringstream ss;
	ss << put_time(std::localtime(&t), "%Y:%m:%d:%H:%M:%S");
	string str_time = ss.str();
	return str_time;
}

//�ַ����Ƿ�ȫ����������ɣ�����ǣ�����true�����򷵻�false
bool isNum(string str)
{
	stringstream sin(str);
	double d;
	char c;
	if (!(sin >> d))return false;
	if (sin >> c)return false;
	return true;
}

//���ַ���ת��Ϊ����
int StrToNum(string str)
{
	int num;
	stringstream ss(str);
	ss >> num;
	return num;
}

//���������Ϣ���ݼӺŽ��зָ�
vector<string> split(string str)
{
	vector<string> elems;
	int pos = 0;
	while (true)
	{
		if (str.find('+') == string::npos)break;
		pos = str.find('+');
		elems.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	elems.push_back(str);
	return elems;
}

//��ȡ���е����֤��Ϣ�ͺ�������Ϣ
void license::readLicenceData(vector<license>& AllLicense)
{
	//��ȡ������
	forbiddenIPs.clear();
	string forbidFilepath = ForbidFILEPATH;
	ifstream fin(forbidFilepath, ios::in);
	if (fin)
	{
		string forbidip;
		while (!fin.eof())
		{
			getline(fin, forbidip);
			forbiddenIPs.push_back(forbidip);
		}
	}

	//��ȡ��Ϣǰ�Ƚ����е���������Ϣ���
	AllLicense.clear();
	vector<string> files;
	vector<int> is_file_packet;
	GetFiles(FILEPATH, files, is_file_packet, 0);

	if (files.size() == 0)cout << "û���κ����֤" << endl;
	else
	{
		string last_username;
		string last_user_password;
		for (int i = 1; i < files.size(); i++)
		{
			int pos = 0;
			pos = files[i].rfind("/");
			string str = files[i].substr(pos + 1, files[i].length());
			if (!isNum(str) && str != "info.txt")
			{
				last_username = str;//��ȡ�û���
				//��ȡ���û���������
				string password_filepath = sourcePath + '/' + last_username + "/info.txt";
				ifstream fin;
				fin.open(password_filepath, ios::in);
				string password;
				getline(fin, password);
				last_user_password = password;
				fin.close();
			}
			if (is_file_packet[i] == 1 && isNum(str))
			{
				license licen;
				licen.seqNum = str;
				licen.username = last_username;
				licen.password = last_user_password;

				i++;
				ifstream fin;
				fin.open(files[i], ios::in);
				string s;
				int num;
				//������ʹ������
				getline(fin, s);
				num = StrToNum(s);
				licen.maxNum = num;
				string current;
				getline(fin, current);//�ڶ��м�¼�Ƿ��ǵ�һ��ʹ��
				if (current == "TRUE")licen.Is_First_Use = true;
				else licen.Is_First_Use = false;
				getline(fin, current);//�����м�¼��ǰʹ������
				num = StrToNum(current);
				licen.currentNum = num;
				//��ȡʹ�õĿͻ���IP+�˿ں�
				if (num > 0)
				{
					for (int j = 0; j < num; j++)
					{
						getline(fin, current);
						licen.Users.push_back(current);
					}

					//��ÿ��IP�Ͷ˿ںŷָ��
					for (int j = 0; j < licen.Users.size(); j++)
					{
						vector<string> userinfo = split((char*)licen.Users[j].c_str());
						string IP = userinfo[0];
						string port = userinfo[1];

						//����¼�û�����Ϣ�浽clientInfo�����ֱ𴴽��߳̽�������
						sockaddr_in addr;
						addr.sin_family = AF_INET;
						addr.sin_addr.s_addr = inet_addr(IP.c_str());
						addr.sin_port = htons(atoi(port.c_str()));
						std::thread* thr = new std::thread(connectClient, addr);
						clientData data(addr, licen.username, licen.password, licen.seqNum, thr);

					}
				}
				fin.close();
				AllLicense.push_back(licen);
			}
		}
	}
}

//������֤��������Ϣ
void license::ShowLicenseInfo(vector<license>& AllLicense)
{
	if (AllLicense.size() > 0)
	{
		cout << endl << "����������������������������������������������������";
		for (int i = 0; i < AllLicense.size(); i++)
		{
			cout << endl << "���к�" << AllLicense[i].seqNum << "�����Ϣ���£�" << endl;
			cout << "�����֤�û���" << AllLicense[i].username;
			cout << "   �û����룺" << AllLicense[i].password << endl;
			cout << "�Ƿ�ʹ�ù���";
			if (AllLicense[i].Is_First_Use == true)cout << "��" << endl;
			else cout << "��" << endl;
			cout << "�������ޣ�" << AllLicense[i].maxNum << " ��ǰʹ��������" << AllLicense[i].currentNum << endl;
			if (AllLicense[i].currentNum > 0)
			{
				cout << "ʹ�ø����֤�Ŀͻ��˵�IP�Ͷ˿ں��Լ���¼ʱ�����£�" << endl;
				for (int j = 0; j < AllLicense[i].Users.size(); j++)
				{
					if (j == AllLicense[i].Users.size() - 1)cout << AllLicense[i].Users[j];
					else cout << AllLicense[i].Users[j] << endl;
				}
				cout << endl;
			}
		}
		cout << "����������������������������������������������������" << endl << endl;
	}
}

void license::GetFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy)
{
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;  //�����õ��ļ���Ϣ��ȡ�ṹ
	string p;  //string�������˼��һ����ֵ����:assign()���кܶ����ذ汾
	if (hierarchy == 0)
	{
		if (_findfirst(p.assign(path).c_str(), &fileinfo) != -1)
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				files.push_back(p.assign(path));
				is_file_packet.push_back(1);
			}
			else
			{
				files.push_back(p.assign(path));
				is_file_packet.push_back(0);
			}
		}
	}

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
		do {
			//cout << fileinfo.name << endl;
			if ((fileinfo.attrib & _A_SUBDIR)) {  //�Ƚ��ļ������Ƿ����ļ���
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
					is_file_packet.push_back(1);
					GetFiles(p.assign(path).append("/").append(fileinfo.name), files, is_file_packet, hierarchy + 1);
				}

			}
			else {
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
				is_file_packet.push_back(0);
			}

		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
		_findclose(hFile);
	}
}


//У���¼��Ϣ
//�����¼����1����¼��Ϣ���󷵻�0���Ѵ�ʹ���������޷���2
//�������к�Ϊ��һ��ʹ�ã����û��������¼��Ϣʱû���������к��򷵻�3
//������4�����ʾ��IP����������������ɵ�¼
int checkInfo(std::string name, std::string pwd, std::string& seqnum)
{
	int result = -1;//�����¼��Ϣ�Ľ��
	//�û����������к�
	if (seqnum != "")
	{
		int i = 0;
		for (; i < AllLicense.size(); i++)
		{
			license licen = AllLicense[i];
			if (licen.username == name && licen.password == pwd && licen.seqNum == seqnum)
			{
				if (licen.currentNum < licen.maxNum)return 1;
				else return 2;
			}
			else if (licen.username == name && licen.password != pwd)
				return 0;
		}
		if (i == AllLicense.size())return 0;
	}
	//�û�û���������к�
	else
	{
		int i = 0;
		int flag = 0;//��¼�û�������û����������Ƿ�ƥ�䣬����2��ʾƥ�䵫�����ﵽ����
		for (; i < AllLicense.size(); i++)
		{
			license licen = AllLicense[i];
			if (licen.username == name && licen.password == pwd)
			{
				flag = 1;
				if (!licen.Is_First_Use)
				{
					if (licen.currentNum < licen.maxNum)
					{
						seqnum = licen.seqNum;
						return 1;
					}
					else flag = 2;
				}
			}
			else if (licen.username == name && licen.password != pwd)
				return 0;
		}
		if (flag == 1 && i == AllLicense.size())return 3;
		if (flag == 2 && i == AllLicense.size())return 2;
	}
}

//�������֤�ļ�
void UpdateLicenseFile(int ser_count)
{
	string filepath;
	filepath = sourcePath + '/' + AllLicense[ser_count].username + '/' +
		AllLicense[ser_count].seqNum + "/info.txt";
	ofstream fout;
	fout.open(filepath, ios::out);
	fout << AllLicense[ser_count].maxNum << endl;
	fout << "FALSE" << endl;
	fout << AllLicense[ser_count].currentNum << endl;
	if (AllLicense[ser_count].Users.size() == 0)fout << "No user";
	for (int j = 0; j < AllLicense[ser_count].Users.size(); j++)
	{
		if (j == AllLicense[ser_count].Users.size() - 1)fout << AllLicense[ser_count].Users[j];
		else fout << AllLicense[ser_count].Users[j] << endl;
	}
	fout.close();
}

void updateLicence(updateTuple tuple)
{
	//��ȡ�ַ�����ʽ��ip+�˿ں�
	stringstream ss;
	ss << ntohs(tuple.clientAddr.sin_port);
	string str_port = ss.str();
	string str_addr = inet_ntoa(tuple.clientAddr.sin_addr);
	string time = GetTime();
	string str = str_addr + "+" + str_port + "+" + time;

	std::string state;
	if (tuple.statusFlag == true)
		state = "login";
	else state = "logout";

	cout << "user " << inet_ntoa(tuple.clientAddr.sin_addr) << " ";
	cout << ntohs(tuple.clientAddr.sin_port) << " " << time << " " << state << std::endl;

	string seqnum = clientInfo[tuple.clientAddr].seqNum;//���и��µ����к�

	//��¼�����
	if (state == "login")
	{
		//�Դ洢���֤��Ϣ���������и���
		int i = 0;
		for (; i < AllLicense.size(); i++)
		{
			if (seqnum == AllLicense[i].seqNum)
			{
				AllLicense[i].currentNum++;
				AllLicense[i].Users.push_back(str);
				AllLicense[i].Is_First_Use = false;
				break;
			}
		}
		//�����֤�ļ����и���
		UpdateLicenseFile(i);
	}
	else
	{
		cout << "�˳�����и������к�" << endl;
		//�Դ洢���֤��Ϣ���������и���
		int i = 0, j = 0;
		for (; i < AllLicense.size(); i++)
		{
			if (seqnum == AllLicense[i].seqNum)
			{
				AllLicense[i].currentNum--;
				for (; j < AllLicense[i].Users.size(); j++)
				{
					int pos = str.rfind('+');
					string s1 = str.substr(0, pos);
					pos = AllLicense[i].Users[j].rfind('+');
					string s2 = AllLicense[i].Users[j].substr(0, pos);
					if (s1 == s2)break;
				}

				//�˳�ʱ�����˳���¼��¼��history.txt����
				string historyFilepath = HistoryFILEPATH;
				ofstream fout(historyFilepath, ios::app);
				fout << AllLicense[i].username << "+" << AllLicense[i].Users[j] << "+" << time << endl;
				fout.close();

				AllLicense[i].Users.erase(AllLicense[i].Users.begin() + j);
				break;
			}
		}
		//�����֤�ļ����и���
		UpdateLicenseFile(i);
		clientInfo.erase(tuple.clientAddr);
	}
	cout << "���֤ʹ��������£����º�ʹ��������£�" << endl;
	myLicense.ShowLicenseInfo(AllLicense);
}


void handleMessage(messageData data) {
	//�������Կͻ��˵���Ϣ
	std::cout << inet_ntoa(data.addr.sin_addr) << ":\t" << data.msg << std::endl;
	std::stringstream ss;
	ss.str(data.msg);
	std::string ins;
	ss >> ins;

	//������Ϣָ��,login��¼,check��ʱ�㱨,quit�˳�
	if (ins == "login")
	{
		std::string username, password, seqNum;	//�û��������룬���к�
		ss >> username >> password >> seqNum;

		int checkResult = checkInfo(username, password, seqNum);

		bool forbidOr = false;//���ں�����Ϊfalse
		//�ȼ��ÿͻ��˵�IP�Ƿ��ں�������
		string thisIP = inet_ntoa(data.addr.sin_addr);
		for (int i = 0; i < forbiddenIPs.size(); i++)
		{
			if (thisIP == forbiddenIPs[i])
			{
				forbidOr = true;
				break;
			}
		}

		//�����������������Ӧ��ʾ
		if (forbidOr)
			myServer.sendToClient(&(data.addr), "forbid");
		else
		{
			if (checkResult == 1)
			{
				//ȷ����Ϣ������ͻ����ȿ��ÿͻ����Ƿ��Ѿ���¼��
				//��ʾ�Ѿ���¼�����Ͳ������µ��߳�
				if (clientInfo.count(data.addr))
				{
					myServer.sendToClient(&(data.addr), "permit");
				}
				//û�е�¼�������͵�¼ָ��
				else
				{
					//�����µ��̹߳�����ͻ��˵����ӣ��������Ӧ�ͻ��˵���Ϣ,����¶�����Ӹ���ָ��
					myServer.sendToClient(&(data.addr), "permit");
					std::thread* thr = new std::thread(connectClient, data.addr);
					clientInfo[data.addr] = clientData(data.addr, username, password, seqNum, thr);
					updateBuffer.push(updateTuple(data.addr, true));
				}
			}
			else if (checkResult == 0)
				myServer.sendToClient(&(data.addr), "inhibit");
			else if (checkResult == 2)
				myServer.sendToClient(&(data.addr), "limited");
			else
				myServer.sendToClient(&(data.addr), "needseqnum");
		}
	}
	else if (ins == "check") {
		//���ݵ�ַ�ҵ���Ӧ���̣߳������߳�ʱ��

		if (clientInfo.count(data.addr)) {
			std::thread* tr = clientInfo.at(data.addr).corrThread;
			DWORD tid = GetThreadId(tr->native_handle());
			while (!PostThreadMessage(tid, CK_MSG, 0, 0));
		}
		//else
		//	myServer.sendToClient(&(data.addr),"offline"); //checkʱ��������߷������ߣ�Ҫ�����µ�¼
	}
	else if (ins == "quit") {
		//�رն�Ӧ�̣߳�����¶�����Ӹ���ָ��
		std::thread* tr = clientInfo.at(data.addr).corrThread;
		DWORD tid = GetThreadId(tr->native_handle());
		while (!PostThreadMessage(tid, WM_QUIT, 0, 0));
		updateBuffer.push(updateTuple(data.addr, false));
	}
	else myServer.sendToClient(&(data.addr), "��������ȷ��ָ�");
}


//��ȡ���������һ���߳�ʹ��
void readcmd()
{
	clock_t time_start = clock();
	while (true)
	{
		clock_t time_end = clock();
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC >= 10)
		{
			fstream f(CMDFILEPATH, ios::in);
			while (!f.eof())//��ȡ��Ϣ
			{
				string temp;
				f >> temp;
				if (temp != "")
					cmdQueue.push(temp);
			}
			f.close();
			f.open(CMDFILEPATH, ios::out);
			f.close();//���
			time_start = clock();
		}
	}
}

vector<string> getCmdItems(string cmd_str)
{
	vector<string> cmd_items;
	int start_pos = 0;
	for (int i = 0; i < cmd_str.length(); i++)
	{
		if (cmd_str[i] == '+')
		{
			cmd_items.push_back(cmd_str.substr(start_pos, i - start_pos));
			start_pos = i + 1;
		}
	}
	cmd_items.push_back(cmd_str.substr(start_pos));
	return cmd_items;
}


int main()
{
	std::stringstream ss;
	myLicense.readLicenceData(AllLicense);
	myLicense.ShowLicenseInfo(AllLicense);
	std::thread recvThread(&server::receieveFromClient, &myServer);
	std::thread cmdThread(readcmd);//�����ȡ�߳�

	while (true) {
		if (!myServer.msgBuffer.empty()) {
			//����ǰ���ȸ���һ�����֤��Ϣ
			myLicense.readLicenceData(AllLicense);

			//�����Ϣ���в�Ϊ�գ�����ȡ��һ����Ϣ���д���
			handleMessage(myServer.msgBuffer.front());
			myServer.msgBuffer.pop();
		}

		if (!updateBuffer.empty()) {
			//������¶��в�Ϊ�գ��ͽ���һ�θ���
			updateLicence(updateBuffer.front());
			updateBuffer.pop();
		}

		if (!cmdQueue.empty())
		{
			while (!cmdQueue.empty())
			{
				string cmd_str = cmdQueue.front();
				cmdQueue.pop();
				vector<string> cmd_items = getCmdItems(cmd_str);
				string cmd_type = cmd_items[0];
				if (cmd_type == "DRP")
				{
					//����ĳ�û�������ʵ�ַ����Ƿ���һ��forbid��Ϣ���ͺ͵�½������һ���ģ�Ȼ��ͻ����յ��Ժ�����
					string company = cmd_items[1];
					string seqNum = cmd_items[2];
					string ip = cmd_items[3];
					string port = cmd_items[4];
					SOCKADDR_IN addr;
					addr.sin_port = htons(atoi(port.c_str()));
					addr.sin_addr.s_addr = inet_addr(ip.c_str());
					addr.sin_family = AF_INET;
					myServer.sendToClient(&addr, "forbid");
					std::thread* tr = clientInfo.at(addr).corrThread;
					DWORD tid = GetThreadId(tr->native_handle());
					while (!PostThreadMessage(tid, WM_QUIT, 0, 0));
					updateBuffer.push(updateTuple(addr, false));
				}
				else if (cmd_type == "UBN")
				{
					//���ĳIP
					string ip = cmd_items[1];
					ofstream out(ForbidFILEPATH, ios::out);
					for (auto it = forbiddenIPs.begin(); it != forbiddenIPs.end(); it++) {
						if (*it != ip)
							out << ip << endl;
						else continue;
					}
				}
				else if (cmd_type == "BAN")
				{
					//���ĳIP
					string ip = cmd_items[1];
					forbiddenIPs.push_back(ip);
					ofstream out(ForbidFILEPATH, ios::app);
					out << ip << endl;
				}
				else
				{
					cout << "bug,check your file" << endl;
				}
			}
		}
	}
	recvThread.join();
	cmdThread.join();
	return 0;
}
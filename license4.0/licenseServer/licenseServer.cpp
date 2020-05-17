#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include"public.h"
#include"server.h"
server myServer;
license myLicense;
std::queue<updateTuple> updateBuffer;	//暂存待进行的更新
std::map<sockaddr_in, clientData> clientInfo;		//使用客户端地址查找客户端信息
vector<license> AllLicense;//存储当前服务器所有的许可证信息

#define FILEPATH "license"
string sourcePath = FILEPATH;

bool operator<(const sockaddr_in& l, const sockaddr_in& r) {
	return l.sin_port < r.sin_port;
}

//字符串是否全部由数字组成，如果是，返回true，否则返回false
bool isNum(string str)
{
	stringstream sin(str);
	double d;
	char c;
	if (!(sin >> d))return false;
	if (sin >> c)return false;
	return true;
}

//将字符串转化为数字
int StrToNum(string str)
{
	int num;
	stringstream ss(str);
	ss >> num;
	return num;
}

//将输入的信息根据空格进行分割
vector<string> split(char *str)
{
	vector<string> elems;
	const char *delim = " ";
	char *s = strtok(str, delim);
	while (s != NULL) {
		elems.push_back(s);
		s = strtok(NULL, delim);
	}
	return elems;
}

//读取所有的许可证信息
void license::readLicenceData(vector<license> &AllLicense) 
{
	//读取信息前先将所有的容器内信息清除
	AllLicense.clear();
	vector<string> files;
	vector<int> is_file_packet;
	GetFiles(FILEPATH, files, is_file_packet, 0);

	if (files.size() == 0)cout << "没有任何许可证" << endl;
	else
	{
		string last_username;
		string last_user_password;
		for (int i = 1;i < files.size();i++)
		{
			int pos = 0;
			pos = files[i].rfind("/");
			string str = files[i].substr(pos + 1, files[i].length());
			if (!isNum(str) && str != "info.txt")
			{
				last_username = str;//获取用户名
				//获取该用户名的密码
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
				//获得最大使用人数
				getline(fin, s);
				num = StrToNum(s);
				licen.maxNum = num;
				string current;
				getline(fin, current);//第二行记录是否是第一次使用
				if (current == "TRUE")licen.Is_First_Use =true;
				else licen.Is_First_Use=false;
				getline(fin, current);//第三行记录当前使用人数
				num = StrToNum(current);
				licen.currentNum = num;
				//存取使用的客户的IP+端口号
				if (num > 0)
				{
					getline(fin, current);
					licen.Users = split((char*)current.c_str());

					//将每个IP和端口号分割开来
					for (int j = 0;j < licen.Users.size();j++)
					{
						int pos = licen.Users[j].find("+");
						string IP = licen.Users[j].substr(0, pos);
						string port = licen.Users[j].substr(pos + 1, str.length());
					}
				}
				fin.close();
				AllLicense.push_back(licen);
			}
		}
	}
}

//输出许可证的所有信息
void license::ShowLicenseInfo(vector<license> &AllLicense)
{
	if (AllLicense.size() > 0)
	{
		cout << endl << "※※※※※※※※※※※※※※※※※※※※※※※※※※";
		for (int i = 0;i < AllLicense.size();i++)
		{
			cout << endl << "序列号" << AllLicense[i].seqNum << "相关信息如下：" << endl;
			cout << "该许可证用户：" << AllLicense[i].username;
			cout << "   用户密码：" << AllLicense[i].password << endl;
			cout << "是否使用过：";
			if (AllLicense[i].Is_First_Use == true)cout << "否" << endl;
			else cout << "是" << endl;
			cout << "人数上限：" << AllLicense[i].maxNum << " 当前使用人数：" << AllLicense[i].currentNum << endl;
			if (AllLicense[i].currentNum > 0)
			{
				cout << "使用该许可证的客户端IP+端口号为：" << endl;
				for (int j = 0;j < AllLicense[i].Users.size();j++)
				{
					if (j == 0)cout << AllLicense[i].Users[j];
					else cout << " " << AllLicense[i].Users[j];
				}
				cout << endl;
			}
		}
		cout << "※※※※※※※※※※※※※※※※※※※※※※※※※※" << endl << endl;
	}
}

void license::GetFiles(string path, vector<string>& files, vector<int>& is_file_packet, int hierarchy)
{
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
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
			if ((fileinfo.attrib & _A_SUBDIR)) {  //比较文件类型是否是文件夹
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

		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}

void connectClient(sockaddr_in clientAddr) {
	//与对应的客户端交流
	//设定一个计时器一段时间没收到check则关闭线程
	clock_t time_start = clock();//开始计时
	clock_t time_end;
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == CK_MSG) {
				//重置计时器
				time_start = clock();
			}

			if (msg.message == WM_QUIT) {
				return;
			}

		}
		time_end = clock();
		//大于等于60秒客户端没有发送check信息就认为客户崩溃，将它踢下线
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC >= 60)
		{
			updateBuffer.push(updateTuple(clientAddr, false));
			return;
		}
	}
}

//校验登录信息
//允许登录返回1，登录信息错误返回0，已达使用人数上限返回2
//若该序列号为第一次使用，且用户在输入登录信息时没有输入序列号则返回3
int checkInfo(std::string name,std::string pwd,std::string seqnum)
{
	int result = -1;//检验登录信息的结果
	//用户输入了序列号
	if (seqnum != "")
	{
		int i = 0;
		for (;i < AllLicense.size();i++)
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
	//用户没有输入序列号
	else
	{
		int i = 0;
		int flag = 0;//记录用户输入的用户名和密码是否匹配
		for (;i < AllLicense.size();i++)
		{
			license licen = AllLicense[i];
			if (licen.username == name && licen.password == pwd)
			{
				flag = 1;
				if (!licen.Is_First_Use)return 1;
			}
			else if (licen.username == name && licen.password != pwd)
				return 0;
		}
		if (flag == 1 && i == AllLicense.size())return 3;
		if (flag == 0 && i == AllLicense.size())return 0;
	}
}

//更新许可证文件
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
	for (int j = 0;j < AllLicense[ser_count].Users.size();j++)
	{
		if (j == 0)fout << AllLicense[ser_count].Users[j];
		else fout << " " << AllLicense[ser_count].Users[j];
	}
	fout.close();
}

void updateLicence(updateTuple tuple) 
{
	std::string state;
	if (tuple.statusFlag == true)
		state = "login";
	else state = "logout";
	cout << "user " << inet_ntoa(tuple.clientAddr.sin_addr) << "+";
	cout << ntohl(tuple.clientAddr.sin_port) << "\t" << state << std::endl;

	//获取字符串形式的ip+端口号
	stringstream ss;
	ss << ntohl(tuple.clientAddr.sin_port);
	string str_port = ss.str();
	string str_addr = inet_ntoa(tuple.clientAddr.sin_addr);
	string str = str_addr + "+" + str_port;

	string seqnum = clientInfo[tuple.clientAddr].seqNum;//进行更新的序列号

	//登录的情况
	if (state == "login")
	{
		//对存储许可证信息的容器进行更新
		int i = 0;
		for (;i < AllLicense.size();i++)
		{
			if (seqnum == AllLicense[i].seqNum)
			{
				AllLicense[i].currentNum++;
				AllLicense[i].Users.push_back(str);
				AllLicense[i].Is_First_Use = false;
				break;
			}
		}
		//对许可证文件进行更新
		UpdateLicenseFile(i);	
	}
	else
	{
		cout << "退出后进行更新序列号" << endl;
		//对存储许可证信息的容器进行更新
		int i = 0, j = 0;
		for (;i < AllLicense.size();i++)
		{
			if (seqnum == AllLicense[i].seqNum)
			{
				AllLicense[i].currentNum--;				
				for (;j < AllLicense[i].Users.size();j++)
				{
					if (AllLicense[i].Users[j] == str)
						break;
				}
				AllLicense[i].Users.erase(AllLicense[i].Users.begin() + j);
				break;
			}
		}
		//对许可证文件进行更新
		UpdateLicenseFile(i);
		clientInfo.erase(tuple.clientAddr);
	}
	cout << "许可证使用情况更新，更新后使用情况如下：" << endl;
	myLicense.ShowLicenseInfo(AllLicense);
}


void handleMessage(messageData data) {
	//处理来自客户端的消息
	std::cout << inet_ntoa(data.addr.sin_addr) << ":\t" << data.msg << std::endl;
	std::stringstream ss;
	ss.str(data.msg);
	std::string ins;
	ss >> ins;

	//三类消息指令,login登录,check定时汇报,quit退出
	if (ins == "login") {
		std::string username, password, seqNum;	//用户名，密码，序列号
		ss >> username >> password >> seqNum;

		int checkResult = checkInfo(username, password, seqNum);

		if (checkResult==1) 
		{
			//确认信息无误向客户端先看该客户端是否已经登录过
			//表示已经登录过，就不创建新的线程
			if (clientInfo.count(data.addr))
			{
				myServer.sendToClient(&(data.addr), "permit");
			}
			//没有登录过，发送登录指令
			else
			{
				//创建新的线程管理与客户端的连接，并储存对应客户端的信息,向更新队列添加更新指令
				myServer.sendToClient(&(data.addr), "permit");
				std::thread* thr = new std::thread(connectClient, data.addr);
				clientInfo[data.addr] = clientData(data.addr, username, password, seqNum, thr);
				updateBuffer.push(updateTuple(data.addr, true));
			}
		}
		else if (checkResult == 0)
			myServer.sendToClient(&(data.addr), "inhibit");
		else if(checkResult==2)
			myServer.sendToClient(&(data.addr), "limited");
		else 
			myServer.sendToClient(&(data.addr), "needseqnum");
	}
	else if (ins == "check") {
		//根据地址找到对应的线程，重置线程时间

		std::thread*tr = clientInfo.at(data.addr).corrThread;
		DWORD tid = GetThreadId(tr->native_handle());
		while (!PostThreadMessage(tid,CK_MSG,0,0));
	}
	else if (ins == "quit") {
		//关闭对应线程，向更新队列添加更新指令
		std::thread* tr = clientInfo.at(data.addr).corrThread;
		DWORD tid = GetThreadId(tr->native_handle());
		while (!PostThreadMessage(tid, WM_QUIT, 0, 0));
		updateBuffer.push(updateTuple(data.addr, false));
	}
	else myServer.sendToClient(&(data.addr), "请输入正确的指令。");
}

int main()
{
	std::stringstream ss;
	myLicense.readLicenceData(AllLicense);
	myLicense.ShowLicenseInfo(AllLicense);
	std::thread recvThread(&server::receieveFromClient, &myServer);

	while (true) {
		if (!myServer.msgBuffer.empty()) {
			//处理前，先更新一遍许可证信息
			myLicense.readLicenceData(AllLicense);

			//如果消息队列不为空，就提取出一条消息进行处理
			handleMessage(myServer.msgBuffer.front());
			myServer.msgBuffer.pop();
		}

		if (!updateBuffer.empty()) {
			//如果更新队列不为空，就进行一次更新
			updateLicence(updateBuffer.front());
			updateBuffer.pop();
		}
	}
	recvThread.join();
	return 0;
}
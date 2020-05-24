#include"public.h"
#include"client.h"
client myClient;
int LogOut = 0;//看是否退出登录，如果退出则为-1

clock_t time_waiting_permit = clock();//上一次得到授权的时间
bool is_ok = true;//程序是否正常运行，如果遇到被踢下线的情况，会设为false，全部线程结束

//允许登录返回1，登录信息错误返回0，已达使用人数上限返回2
//若该序列号为第一次使用，且用户在输入登录信息时没有输入序列号则返回3
int login(std::string username, std::string password, std::string seqNum) {
	//向服务器发送信息登录
	myClient.sendToServer("login " + username + " " + password + " " + seqNum);
	std::string reply = myClient.receieveFromServer();
	if (reply == "permit")
		return 1;
	else if (reply == "limited")
		return 2;
	else if (reply == "needseqnum")
		return 3;
	else if (reply == "forbid")
		return 4;
	else
		return 0;
}

//每3分钟向服务端发送一个check
void check() 
{
	clock_t time_start = clock();
	while (true)
	{
		if (!is_ok)
			return;
		if ((time_waiting_permit - time_start) / (double)CLOCKS_PER_SEC >= 1800)//长时间没收到服务器授权，认为服务器崩溃
		{
			cout << "服务器长时间没授权，服务器崩溃";
			is_ok = false;
		}
		
		if (LogOut == -1)break;
		clock_t time_end = clock();
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC >= 180)
		{
			myClient.sendToServer("check");
			time_start = clock();
		}
	}
	LogOut = 0;
}

//判断输入的字符串是否全部由数字组成，如果是，返回true，否则返回false
bool isNum(string seqNum)
{
	stringstream sin(seqNum);
	double d;
	char c;
	if (!(sin >> d))return false;
	if (sin >> c)return false;
	return true;
}

void checkInput(string &username, string &password, string &seqNum)
{
	string input;
	getline(cin, input, '\n');

	//检验输入的序列号是否全为数字并且长度为10
	while (true)
	{
		vector<string> info = myClient.split((char*)input.c_str());
		if (info.size() < 2 || info.size() > 3)
		{
			cout << "登录应至少输入用户名和密码" << endl;
			cout << "请按照：[用户名] [密码] [序列号](选填)的格式重新输入：" << endl;
			input.clear();
			getline(cin, input, '\n');
			continue;
		}
		else if (info.size() == 3)
		{
			seqNum = info[2];
			if (!isNum(seqNum))
			{
				cout << "序列号必须全部是数字，请按照：[用户名] [密码] [序列号](选填)的格式重新输入：" << endl;
				input.clear();
				getline(cin, input, '\n');

				continue;
			}
			else if (seqNum.length() != 10)
			{
				cout << "序列号长度为10，请按照：[用户名] [密码] [序列号](选填)的格式重新输入：" << endl;
				input.clear();
				getline(cin, input, '\n');

				continue;
			}
			else
			{
				username = info[0];
				password = info[1];
				break;
			}
		}
		//只输入了用户名和密码则不需要校验
		else
		{
			username = info[0];
			password = info[1];
			seqNum = "";
			break;
		}
	}
}

void clientlistenr()
{
	while (true)
	{
		if (!is_ok)
			return;
		std::string reply = myClient.receieveFromServer();//接受返回
		if (reply == "permit")
		{
			time_waiting_permit = clock();//刷新持续时间
		}
		else if (reply == "forbid")
		{
			cout << "您被强制下线" << endl;
			is_ok = false;
		}
		else
			cout << "存在bug，报告状态时收到了意外的返回内容" << endl;
		if (is_ok == false)
			exit(-1);
	}

}

int main()
{
	std::string username, password, seqNum;

	while (true)
	{
		printf("要登录请按照：[用户名] [密码] [序列号](选填)的格式输入\n");

		checkInput(username, password, seqNum);

		int logResult = login(username, password, seqNum);
		while (logResult != 1)
		{
			if (logResult == 0)
				printf("登陆失败，请重新输入\n按照：[用户名] [密码] [序列号]的格式输入\n");
			else if (logResult == 2)
				cout << "当前许可证使用人数已经到达上限，请尝试稍后再登录或者购买新的许可证" << endl;
			else if (logResult == 3)
			{
				cout << "当前用户空闲的许可证为第一次使用，请重新输入" << endl;
				cout << "按照：[用户名] [密码] [序列号]的格式输入" << endl;
			}
			else if (logResult == 4)
				cout << "您使用的IP被拉入黑名单，不可使用该软件，请联系客服解封" << endl;


			checkInput(username, password, seqNum);

			logResult = login(username, password, seqNum);
		}
		printf("登录成功\n");

		std::thread check_thread(check);
		check_thread.detach();//将子线程与主线程分离
		std::thread listener_thread(clientlistenr);//监听服务器返回的信息，并通过设置参数is_ok确定程序是否被结束

		while (true) {
			printf("输入quit退出\n");
			string INS;
			getline(cin, INS, '\n');
			if (INS == "quit")
			{
				myClient.sendToServer(INS);
				break;
			}
		}
		cout << "成功退出" << endl;
		LogOut = -1;
	}
	return 0;
}
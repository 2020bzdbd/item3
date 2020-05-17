#include"public.h"
#include"client.h"
client myClient;
int LogOut = 0;//看是否退出登录，如果退出则为-1

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
	else return 0;
}

//每30秒向服务端发送一个check
void check() {
	clock_t time_start = clock();
	while (true)
	{
		if (LogOut == -1)break;
		clock_t time_end = clock();
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC == 30)
		{
			myClient.sendToServer("check");
			time_start = clock();
		}
	}
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

int main()
{
	std::string username, password, seqNum;
	printf("请按照：[用户名] [密码] [序列号]的格式输入\n");
	std::cin >> username >> password >> seqNum;

	//检验输入的序列号是否全为数字并且长度为10
	while (true)
	{
		if (!isNum(seqNum))
		{
			cout << "序列号必须全部是数字，请按照：[用户名] [密码] [序列号]的格式重新输入：" << endl;
			cin>> username >> password >> seqNum;
			continue;
		}
		else if (seqNum.length() != 10)
		{
			cout << "序列号长度为10，请按照：[用户名] [密码] [序列号]的格式重新输入：" << endl;
			cin >> username >> password >> seqNum;
			continue;
		}
		else break;
	}

	int logResult = login(username, password, seqNum);
	while (logResult != 1)
	{
		if (logResult == 0)
			printf("登陆失败，请重新输入\n按照：[用户名] [密码] [序列号]的格式输入\n");
		else if (logResult == 2)
			cout << "当前用户所有的许可证均已经达到使用人数上限，请尝试稍后再登录或者购买新的许可证" << endl;
		else
		{
			cout << "当前用户空闲的许可证为第一次使用，请重新输入" << endl;
			cout << "按照：[用户名] [密码] [序列号]的格式输入" << endl;
		}
		cin >> username >> password >> seqNum;
		logResult = login(username, password, seqNum);
	}
	printf("登录成功\n");

	std::thread check_thread(check);
	check_thread.detach();//将子线程与主线程分离

	while (true) {
		printf("输入quit退出\n");
		std::string INS;
		std::cin >> INS;
		if (INS == "quit") {
			myClient.sendToServer(INS);
			break;
		}
	}
	LogOut = -1;
	system("pause");
	return 0;
}
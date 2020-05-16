#include"public.h"
#include"client.h"
client myClient;

bool login(std::string username, std::string password, std::string seqNum) {
	//向服务器发送信息登录
	myClient.sendToServer("login " + username + " " + password + " " + seqNum);
	std::string reply = myClient.receieveFromServer();
	if (reply == "permit")
		return true;
	else return false;
}

void check() {
	//每经过一段时间对服务器发一个check
}

int main()
{
	std::string username, password, seqNum;
	printf("请按照：[用户名] [密码] [序列号]的格式输入\n");
	std::cin >> username >> password >> seqNum;
	while (!login(username, password, seqNum)) {
		printf("登陆失败，请重新输入\n按照：[用户名] [密码] [序列号]的格式输入\n");
		std::cin >> username >> password >> seqNum;
	}
	printf("登录成功\n");
	//std::thread check_thread(check);
	while (true) {
		printf("输入quit退出\n");
		std::string INS;
		std::cin >> INS;
		if (INS == "quit") {
			myClient.sendToServer(INS);
			break;
		}
	}

	system("pause");
	return 0;
}
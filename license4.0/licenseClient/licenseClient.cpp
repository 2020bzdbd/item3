#include"public.h"
#include"client.h"
client myClient;
int LogOut = 0;//���Ƿ��˳���¼������˳���Ϊ-1

//�����¼����1����¼��Ϣ���󷵻�0���Ѵ�ʹ���������޷���2
//�������к�Ϊ��һ��ʹ�ã����û��������¼��Ϣʱû���������к��򷵻�3
int login(std::string username, std::string password, std::string seqNum) {
	//�������������Ϣ��¼
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

//ÿ30�������˷���һ��check
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

//�ж�������ַ����Ƿ�ȫ����������ɣ�����ǣ�����true�����򷵻�false
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
	printf("�밴�գ�[�û���] [����] [���к�]�ĸ�ʽ����\n");
	std::cin >> username >> password >> seqNum;

	//������������к��Ƿ�ȫΪ���ֲ��ҳ���Ϊ10
	while (true)
	{
		if (!isNum(seqNum))
		{
			cout << "���кű���ȫ�������֣��밴�գ�[�û���] [����] [���к�]�ĸ�ʽ�������룺" << endl;
			cin>> username >> password >> seqNum;
			continue;
		}
		else if (seqNum.length() != 10)
		{
			cout << "���кų���Ϊ10���밴�գ�[�û���] [����] [���к�]�ĸ�ʽ�������룺" << endl;
			cin >> username >> password >> seqNum;
			continue;
		}
		else break;
	}

	int logResult = login(username, password, seqNum);
	while (logResult != 1)
	{
		if (logResult == 0)
			printf("��½ʧ�ܣ�����������\n���գ�[�û���] [����] [���к�]�ĸ�ʽ����\n");
		else if (logResult == 2)
			cout << "��ǰ�û����е����֤���Ѿ��ﵽʹ���������ޣ��볢���Ժ��ٵ�¼���߹����µ����֤" << endl;
		else
		{
			cout << "��ǰ�û����е����֤Ϊ��һ��ʹ�ã�����������" << endl;
			cout << "���գ�[�û���] [����] [���к�]�ĸ�ʽ����" << endl;
		}
		cin >> username >> password >> seqNum;
		logResult = login(username, password, seqNum);
	}
	printf("��¼�ɹ�\n");

	std::thread check_thread(check);
	check_thread.detach();//�����߳������̷߳���

	while (true) {
		printf("����quit�˳�\n");
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
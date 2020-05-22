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
	else if (reply == "forbid")
		return 4;
	else
		return 0;
}

//ÿ3���������˷���һ��check
void check() 
{
	clock_t time_start = clock();
	while (true)
	{
		if (LogOut == -1)break;
		clock_t time_end = clock();
		if ((time_end - time_start) / (double)CLOCKS_PER_SEC == 180)
		{
			myClient.sendToServer("check");
			time_start = clock();
		}
	}
	LogOut = 0;
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

void checkInput(string &username, string &password, string &seqNum)
{
	string input;
	getline(cin, input, '\n');

	//������������к��Ƿ�ȫΪ���ֲ��ҳ���Ϊ10
	while (true)
	{
		vector<string> info = myClient.split((char*)input.c_str());
		if (info.size() < 2 || info.size() > 3)
		{
			cout << "��¼Ӧ���������û���������" << endl;
			cout << "�밴�գ�[�û���] [����] [���к�](ѡ��)�ĸ�ʽ�������룺" << endl;
			input.clear();
			getline(cin, input, '\n');
			continue;
		}
		else if (info.size() == 3)
		{
			seqNum = info[2];
			if (!isNum(seqNum))
			{
				cout << "���кű���ȫ�������֣��밴�գ�[�û���] [����] [���к�](ѡ��)�ĸ�ʽ�������룺" << endl;
				input.clear();
				getline(cin, input, '\n');

				continue;
			}
			else if (seqNum.length() != 10)
			{
				cout << "���кų���Ϊ10���밴�գ�[�û���] [����] [���к�](ѡ��)�ĸ�ʽ�������룺" << endl;
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
		//ֻ�������û�������������ҪУ��
		else
		{
			username = info[0];
			password = info[1];
			seqNum = "";
			break;
		}
	}
}

int main()
{
	std::string username, password, seqNum;

	while (true)
	{
		printf("Ҫ��¼�밴�գ�[�û���] [����] [���к�](ѡ��)�ĸ�ʽ����\n");

		checkInput(username, password, seqNum);

		int logResult = login(username, password, seqNum);
		while (logResult != 1)
		{
			if (logResult == 0)
				printf("��½ʧ�ܣ�����������\n���գ�[�û���] [����] [���к�]�ĸ�ʽ����\n");
			else if (logResult == 2)
				cout << "��ǰ���֤ʹ�������Ѿ��������ޣ��볢���Ժ��ٵ�¼���߹����µ����֤" << endl;
			else if (logResult == 3)
			{
				cout << "��ǰ�û����е����֤Ϊ��һ��ʹ�ã�����������" << endl;
				cout << "���գ�[�û���] [����] [���к�]�ĸ�ʽ����" << endl;
			}
			else if (logResult == 4)
				cout << "��ʹ�õ�IP�����������������ʹ�ø����������ϵ�ͷ����" << endl;


			checkInput(username, password, seqNum);

			logResult = login(username, password, seqNum);
		}
		printf("��¼�ɹ�\n");

		std::thread check_thread(check);
		check_thread.detach();//�����߳������̷߳���

		while (true) {
			printf("����quit�˳�\n");
			string INS;
			getline(cin, INS, '\n');
			if (INS == "quit")
			{
				myClient.sendToServer(INS);
				break;
			}
		}
		cout << "�ɹ��˳�" << endl;
		LogOut = -1;
	}
	return 0;
}
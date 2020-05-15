#include"public.h"
#include"server.h"
server myServer;
std::queue<updateTuple> updateBuffer;	//�ݴ�����еĸ���
std::map<sockaddr_in, clientData> clientInfo;		//ʹ�ÿͻ��˵�ַ���ҿͻ�����Ϣ


void connectClient(sockaddr_in clientAddr) {
	//���Ӧ�Ŀͻ��˽���
}

bool checkInfo(std::string name,std::string pwd,std::string seqnum){
	//ȷ�ϵ�¼��Ϣ
}

void updateLicence(updateTuple tuple) {
	//��֤�����ݽ��и���
}

void handleMessage(messageData data) {
	//�������Կͻ��˵���Ϣ
	std::stringstream ss;
	ss.str(data.msg);
	std::string ins;
	ss >> ins;

	//������Ϣָ��,login��¼,check��ʱ�㱨,quit�˳�
	if (ins == "login") {
		std::string username, password, seqNum;	//�û��������룬���к�
		ss >> username >> password >> seqNum;

		if (checkInfo(username, password, seqNum)) {
			//ȷ����Ϣ������ͻ��˷��������¼ָ��
			//�����µ��̹߳�����ͻ��˵����ӣ��������Ӧ�ͻ��˵���Ϣ,����¶�����Ӹ���ָ��
			myServer.sendToClient(&(data.addr), "permit");
			std::thread* thr = new std::thread(connectClient, data.addr);
			clientInfo[data.addr] = clientData(data.addr,username,password,seqNum,thr);
			updateBuffer.push(updateTuple(data.addr, true));
		}
		else myServer.sendToClient(&(data.addr), "inhibit");
	}
	else if (ins == "check") {
		//���ݵ�ַ�ҵ���Ӧ���̣߳������߳�ʱ��
	}
	else if (ins == "quit") {
		//��ͻ��˷�����Ϣ,�յ��ظ���һ��ʱ���رն�Ӧ�̣߳�����¶�����Ӹ���ָ��
	}
	else myServer.sendToClient(&(data.addr), "��������ȷ��ָ�");
}

int main()
{
	std::stringstream ss;
	std::thread recvThread(&server::receieveFromClient, myServer);
	while (true) {

		if (!myServer.msgBuffer.empty()) {
			//�����Ϣ���в�Ϊ�գ�����ȡ��һ����Ϣ���д���
			handleMessage(myServer.msgBuffer.front());
			myServer.msgBuffer.pop();
		}

		if (!updateBuffer.empty()) {
			//������¶��в�Ϊ�գ��ͽ���һ�θ���
			updateLicence(updateBuffer.front());
			updateBuffer.pop();
		}

	}

	recvThread.join();
	return 0;
}
#include"public.h"
#include"server.h"
server myServer;
std::queue<updateTuple> updateBuffer;	//暂存待进行的更新
std::map<sockaddr_in, clientData> clientInfo;		//使用客户端地址查找客户端信息


void connectClient(sockaddr_in clientAddr) {
	//与对应的客户端交流
}

bool checkInfo(std::string name,std::string pwd,std::string seqnum){
	//确认登录信息
}

void updateLicence(updateTuple tuple) {
	//对证件内容进行更新
}

void handleMessage(messageData data) {
	//处理来自客户端的消息
	std::stringstream ss;
	ss.str(data.msg);
	std::string ins;
	ss >> ins;

	//三类消息指令,login登录,check定时汇报,quit退出
	if (ins == "login") {
		std::string username, password, seqNum;	//用户名，密码，序列号
		ss >> username >> password >> seqNum;

		if (checkInfo(username, password, seqNum)) {
			//确认信息无误向客户端发送允许登录指令
			//创建新的线程管理与客户端的连接，并储存对应客户端的信息,向更新队列添加更新指令
			myServer.sendToClient(&(data.addr), "permit");
			std::thread* thr = new std::thread(connectClient, data.addr);
			clientInfo[data.addr] = clientData(data.addr,username,password,seqNum,thr);
			updateBuffer.push(updateTuple(data.addr, true));
		}
		else myServer.sendToClient(&(data.addr), "inhibit");
	}
	else if (ins == "check") {
		//根据地址找到对应的线程，重置线程时间
	}
	else if (ins == "quit") {
		//向客户端发送消息,收到回复或一定时间后关闭对应线程，向更新队列添加更新指令
	}
	else myServer.sendToClient(&(data.addr), "请输入正确的指令。");
}

int main()
{
	std::stringstream ss;
	std::thread recvThread(&server::receieveFromClient, myServer);
	while (true) {

		if (!myServer.msgBuffer.empty()) {
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
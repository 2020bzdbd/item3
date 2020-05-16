#include"public.h"
#include"server.h"
server myServer;
std::queue<updateTuple> updateBuffer;	//暂存待进行的更新
std::map<sockaddr_in, clientData> clientInfo;		//使用客户端地址查找客户端信息

bool operator<(const sockaddr_in& l, const sockaddr_in& r) {
	return l.sin_port < r.sin_port;
}

void readLicenceData() {
	//读取证件中的信息
}

void connectClient(sockaddr_in clientAddr) {
	//与对应的客户端交流
	//设定一个计时器一段时间没收到check则关闭线程
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == CK_MSG) {
				//重置计时器
			}

			if (msg.message == WM_QUIT) {
				return;
			}

		}
	}


}

bool checkInfo(std::string name,std::string pwd,std::string seqnum){
	//确认登录信息

	return true;
}

void updateLicence(updateTuple tuple) {
	//对证件文件内容进行更新
	std::string state;
	if (tuple.statusFlag == true)
		state = "login";
	else state = "logout";
	std::cout << "user " << inet_ntoa(tuple.clientAddr.sin_addr) << "\t" << state << std::endl;
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

		std::thread*tr = clientInfo.at(data.addr).corrThread;
		DWORD tid = GetThreadId(tr->native_handle());
		while (!PostThreadMessage(tid,CK_MSG,0,0));
	}
	else if (ins == "quit") {
		//关闭对应线程，向更新队列添加更新指令

		std::thread* tr = clientInfo.at(data.addr).corrThread;
		DWORD tid = GetThreadId(tr->native_handle());
		while (!PostThreadMessage(tid, WM_QUIT, 0, 0));
		clientInfo.erase(data.addr);
		updateBuffer.push(updateTuple(data.addr, false));

	}
	else myServer.sendToClient(&(data.addr), "请输入正确的指令。");
}

int main()
{
	std::stringstream ss;
	readLicenceData();
	std::thread recvThread(&server::receieveFromClient, &myServer);

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
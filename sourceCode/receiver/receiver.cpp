#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "public.h"

int main()
{
	server ser;
	ser.process();

	return 0;
}

//构造函数，设置服务端信息
server::server()
{
	listener = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//将字符串类型转换uint32_t
}

//初始化函数，功能创建监听套接字，绑定端口，并进行监听
void server::init()
{
	int   Ret;
	WSADATA   wsaData;// 用于初始化套接字环境

	//初始化WinSock环境
	//失败时
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup()   failed   with   error   %d\n", Ret);
		WSACleanup();
	}

	//初始化WinSock环境成功
	listener = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4,TCP传输
	if (listener == -1) 
	{ 
		printf("Error at socket(): %ld\n", WSAGetLastError()); 
		perror("创建失败"); 
		exit(1); 
	}
	cout << "创建成功" << endl;

	unsigned long ul = 1;
	if (ioctlsocket(listener, FIONBIO, (unsigned long*)&ul) == -1) 
	{ 
		perror("ioctl failed"); 
		exit(1); 
	}
	
	if (bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	if (listen(listener, 6) < 0) 
	{ 
		perror("listen failed"); 
		exit(1); 
	}
	socnum.push_back(listener);//将监听套接字加入
}

void server::process()
{
	int mount = 0;
	fd_set fds;
	FD_ZERO(&fds);//将fds清零

	//初始化，与服务端建立连接
	init();

	cout << "正在等待客户端信息......" << endl;
	while (1)
	{
		mount = socnum.size();
		//将fds每次都重新赋值
		for (int i = 0; i < mount; i++)
		{
			FD_SET(socnum[i], &fds);
		}

		struct timeval timeout = { 1,0 };//设置每隔1秒select一次
		//服务端只设置读，不考虑写
		switch (select(0, &fds, NULL, NULL, &timeout))
		{
		case -1:
		{
			perror("select\n");
			printf("Error at socket(): %ld\n", WSAGetLastError());
			printf("%d\n", mount);
			
			Sleep(1000);
			break;
		}
		case 0:
		{
			break;
		}
		default:
		{
			//将数组中的每一个套接字都和剩余的套接字进行比较得到当前的任务
			for (int i = 0; i < mount; i++)
			{
				//如果第一个有套接字可读的消息，就建立建立连接
				if (i == 0 && FD_ISSET(socnum[i], &fds))
				{
					struct sockaddr_in client_address;
					socklen_t client_addrLength = sizeof(struct sockaddr_in);
					//返回一个用户的套接字
					int clientfd = accept(listener, (struct sockaddr*)&client_address, &client_addrLength);
					//添加用户，服务器上显示消息，并通知用户连接成功
					socnum.push_back(clientfd);
					cout << "客户端 " << clientfd << "成功连接本服务器" << endl;
					char ID[1024];
					sprintf(ID, "你好，你的ID为：%d", clientfd);
					
					//服务器产生ID并发送给客户端让客户端知道自己的ID
					send(clientfd, ID, sizeof(ID) - 1, 0);//减去最后一个'/0'
				}
				if (i != 0 && FD_ISSET(socnum[i], &fds))
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));
					int size = recv(socnum[i], buf, sizeof(buf) - 1, 0);
					//检测是否断线
					if (size == 0 || size == -1)
					{
						cout << "客户端 " << socnum[i] << "已掉线" << endl;

						closesocket(socnum[i]);//关闭这个套接字
						FD_CLR(socnum[i], &fds);//在列表列表中删除

						socnum.erase(socnum.begin() + i);//在vector数组中删除
						mount--;
					}
					//若没有掉线
					else
					{
						printf("客户端 %d 说: %s \n", socnum[i], buf);

						//对用户发送过来的信息进行分割，提取出（序列号），用户名，密码
						vector<string>info = split(buf);
						string str;
						if (info.size() == 2)
						{
							info[0] = "用户名：" + info[0];
							info[1] = "密码：" + info[1];
							str = info[0] + " " + info[1];
						}
						else
						{
							info[0] = "序列号：" + info[0];
							info[1] = "用户名：" + info[1];
							info[2] = "密码：" + info[2];
							str = info[0] + " " + info[1] + " " + info[2];
						}

						/*
						提取完三个（二个）信息后，进行校验，校验的方法应该加在这里
						*/

						//将消息发送给发来消息的客户端
						char client[1024];
						sprintf(client, "客户端 %d，", socnum[i]);
						strcat(client, str.c_str());
						
						send(socnum[i], client, sizeof(client) - 1, 0);//如果
					}
				}
			}
			break;
		}
		}
	}
}

//将输入的信息根据空格进行分割
vector<string> server::split(char *str)
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
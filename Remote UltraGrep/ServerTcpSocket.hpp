#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
using namespace std;

#pragma comment (lib,"ws2_32.lib")

unsigned short constexpr PORT = 27015;

class ServerTcpSocket {
	SOCKET hSocket;
	sockaddr_in serverAddress;

public:
	int initialize();
	void createSocket();
	void createAddress(PCSTR const& address);
	int bindSocketandAddress();
	int startListen();
	void communication();
	void clean();
};
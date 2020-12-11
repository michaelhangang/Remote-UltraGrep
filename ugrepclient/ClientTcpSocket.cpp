#include "ClientTcpSocket.hpp"

int ClientTcpSocket::initialize() {
	cout << "TCPClient\n";
	WSAData wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cerr << "WSAStartup failed: " << iResult << endl;
		return  EXIT_FAILURE;
	}
	return 0;
}

void ClientTcpSocket::createSocket() {
	 hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void ClientTcpSocket::createAddress(PCSTR const &address) {
	serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	inet_pton(AF_INET, address, &(serverAddress.sin_addr));

	
}

int ClientTcpSocket::connectServer() {
	if (connect(hSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		cerr << "connect() failed\n";
		clean();
		return EXIT_FAILURE;
	
	}
	cerr << "connect() success\n";
	return 0;
}


string ClientTcpSocket::communication(PCSTR const& sendbuf) {
	char recvbuf[8555]="";
	int bytesSent = send(hSocket, sendbuf, (int)strlen(sendbuf), 0);
	int bytesRecv = recv(hSocket, recvbuf, 8555, 0);
	string recvStr = recvbuf;
	return recvStr;
}

void ClientTcpSocket::clean() {
	cout << "Closing the client socket.\n";
	closesocket(hSocket);
	WSACleanup();
}

#include "ServerTcpSocket.hpp"
#include "UltraGrep.cpp"

int ServerTcpSocket::initialize() {
	cout << "TCPServer\n";
	WSAData wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cerr << "WSAStartup failed: " << iResult << endl;
		return EXIT_FAILURE;
	}
	return 0;
}

void ServerTcpSocket::createSocket() {
	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void ServerTcpSocket::createAddress(PCSTR const& address) {
	serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	inet_pton(AF_INET, address, &(serverAddress.sin_addr));


}

int ServerTcpSocket::bindSocketandAddress() {
	
	if (::bind(hSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		cerr << "Bind() failed" << endl;
		clean();
		return EXIT_FAILURE;
	}

	cout << "TCP/IP socket bound\n";
	return 0;
}

int ServerTcpSocket::startListen() {
	if (listen(hSocket, 10) == SOCKET_ERROR) {
		cerr << "Error listening on socket\n";
		clean();
		return EXIT_FAILURE;
	}
	return 0;
}

void ServerTcpSocket::communication() {
	cout << "Waiting for connection\n";
	SOCKET hAccepted = SOCKET_ERROR;
	while (hAccepted == SOCKET_ERROR)
		hAccepted = accept(hSocket, NULL, NULL);
	cout << "Client connected\n";
	UltraGrep ultraGrep;
	
	for (;;)
	{
		int bytesSent;
		char recvbuf[255] = "";
		int bytesRecv = recv(hAccepted, recvbuf, 255, 0);
		cout << "Recv = " << bytesRecv << ": " << recvbuf << endl;
		if(bytesRecv==-1)
			throw exception("Client did not send a message. The connection is closed!");
		
		if (!strcmp(recvbuf, ":quit")) {
			string const terminateMsg = "server exit";
			send(hAccepted, terminateMsg.c_str(), (int)terminateMsg.size(), 0);

			// terminate
			cout << "Closing the accepted socket.\n";
			closesocket(hAccepted);
			break;
		}
		string msg = recvbuf;
		string const result = ultraGrep.process(msg);
		bytesSent = send(hAccepted, result.c_str(), (int)result.size() + 1, 0);
		cout << "Send = " << bytesSent << endl;

	}
}

void ServerTcpSocket::clean() {
	cout << "Closing the listening socket.\n";
	closesocket(hSocket);
	WSACleanup();
}
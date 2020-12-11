// Remote UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Author : Gang Han
//Data: 2020 12.10
#include "ServerTcpSocket.hpp"

#include <string>
using namespace std;

int main(int argc, char* argv[])
{
    if (argc > 2) {
        cout << "Please enter ugrepserver [ip]" << endl;
        return EXIT_FAILURE;
    }

    ServerTcpSocket server;
    int result = server.initialize();
    if (result == EXIT_FAILURE)
        return EXIT_FAILURE;
  
    server.createSocket();

    if (argc == 2) {
        string ip = argv[1];
        server.createAddress(ip.c_str());
    }
    else {
        server.createAddress("127.0.0.1");
    }
   

    try
    {
        if (server.bindSocketandAddress() == EXIT_FAILURE)
           return EXIT_FAILURE;
  
        if (server.startListen() == EXIT_FAILURE)
            return EXIT_FAILURE;
        server.communication();
    }
    catch (const exception&e)
    {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Error: an unknown exception has been caught by main()\n";
        return EXIT_FAILURE;
    }
    server.clean();
}


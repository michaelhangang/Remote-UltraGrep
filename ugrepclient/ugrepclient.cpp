// ugrepclient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Author : Gang Han
//Data: 2020 12.10

#include "ClientTcpSocket.hpp"
#include <string>
#include <thread>
#include <future>
#include <sstream>
using namespace std;


// Get a report from a server.
void getReport(promise<string>& p, string& input, ClientTcpSocket& client) {
    string results = client.communication(input.c_str());
    p.set_value(results);   
}

// Validate the command
bool validateCommend(string const& command) {
    istringstream iss(command);
    string line;
    vector<string> commands;
    while (getline(iss, line, ' ')) {
        if (line != "") {
            commands.push_back(line);
        }
    }
    if (commands.size() != 3) {
        cout << "The number of the commands was not correct!" << endl;
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    if (argc > 2 ) {
        cout << "Please enter ugrepclient [ip]" << endl;
        return EXIT_FAILURE;
    }

     ClientTcpSocket client;
     int result = client.initialize();
     if (result == EXIT_FAILURE)
         return EXIT_FAILURE;

      client.createSocket();

    if (argc == 2) {
        string ip =  argv[1];     
        client.createAddress(ip.c_str());
    }
    else {
        client.createAddress("127.0.0.1");
    }
   
    int reponse = client.connectServer();
    if (reponse == EXIT_FAILURE)
        return EXIT_FAILURE;
   
    cout << "Please enter grep command - remotefolder regex .exts : ";
    string line;   
    try
    {
        while (getline(cin, line))
        {
            // If command is not ':quit', it will be validated.
            if (line != ":quit") {
                if (validateCommend(line)) {     // Check if the number of the commands is correct.
                    cout << "Please enter grep command - remotefolder regex .exts : ";
                    continue;
                }
            }
            
            // Creat a seperate thread for the communication
            promise<string> p;
            future<string> promiseFuture = p.get_future();
            thread(&getReport, ref(p), ref(line), ref(client)).detach();
            // Wait for the result
            promiseFuture.wait();
            auto results = promiseFuture.get();

            if (results != "") {
                // Check if the server terminated
                if (results == "server exit") {
                    cout << "Server terminated" << endl;
                    break;
                }
                cout << results << endl;
              
            }
            else
                cout << "No result was produced." << endl;
            cout << "Please enter grep command - remotefolder regex .exts : ";

        }
    }
    catch (const std::exception&e)
    {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Error: an unknown exception has been caught by main()\n";
        return EXIT_FAILURE;
    }
    client.clean();
}


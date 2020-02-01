#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

/*

Client:
	https://youtu.be/0Zr_0Jy8mWE

*/

int main()
{
	//Initialze winsock
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 10);
	bool conn = false;
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	SetConsoleTitle("*server*");
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Cant't Intialize winsock! Quitting" << endl;
		return 0;
	}

	// create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return 0;
	}

	// Bind the ip address and port to a socket

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;

	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	string thisHost;
	string thisService;
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
		conn = true;
		thisHost = host;
		thisService = service;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
		conn = true;
		thisHost = host;
		thisService = service;
	}

	//Close listening socket
	closesocket(listening);

	//While loop: accept and echo message back to client
	char buf[4096];
	string userInput;
	bool wh = true;
	cout << "Commands:" << endl << endl;
	cout << "echo title [title]" << endl;
	cout << "echo msg [message]" << endl;
	cout << "exit_user" << endl;
	cout << "exit" << endl;
	cout << "console show" << endl;
	cout << "console hide" << endl;
	cout << "console clear" << endl;
	cout << "cursor [freze|setlocation|move]" << endl;
	cout << "shutdown" << endl;
	cout << "reload" << endl;
	cout << "shutdown" << endl;
	cout << endl;
	string exit_user = "exit_user";
	do
	{
		cout << "> ";
		getline(cin, userInput);

		if (userInput == "exit") {
			send(clientSocket, exit_user.c_str(), exit_user.size() + 1, 0);
			closesocket(clientSocket);
			WSACleanup();
			system("cls");
			return 0;
		}
		else if (userInput == "reload")
		{
			closesocket(clientSocket);
			WSACleanup();
			system("cls");
			return main();
		}

		if (userInput.size() > 0)
		{
			int sendResult = send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				if (userInput == "exit_user")
				{
					closesocket(clientSocket);
					WSACleanup();
					system("cls");
					return main();
				}
			}
		}
	} while (userInput.size() > 0);

	// Close the Socket
	closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
	return 0;
}
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	std::string ipAddress = "192.168.178.14";	// IP Address of the server
	int port = 54000;							// Listening port # on server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err#" << wsResult << std::endl;
		std::cin.get();
		return 0;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err#" << WSAGetLastError() << std::endl;
		WSACleanup();
		std::cin.get();
		return 0;
	}
	
	// Fill in a hint struture (tells winsock what server and port we want to connect to)
	sockaddr_in hint; //ip4 version of the structure
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); //host to network short
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Error#" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		std::cin.get();
		return 0;
	}

	// Do-while loop to send and receive data
	char buf[4096];
	std::string userInput;

	do
	{
		// Prompt the user for some text
		std::cout << "> ";
		std::getline(std::cin, userInput);
		if (userInput.size() > 0) // Make sure the user has typed somethingn
		{
			// Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);//string is terminated with a 0 character
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to the console
					std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
				}
				
			}
		}
	} while (userInput.size() > 0);

	// Close down
	closesocket(sock);
	WSACleanup();


	std::cin.get();
	return 0;
}
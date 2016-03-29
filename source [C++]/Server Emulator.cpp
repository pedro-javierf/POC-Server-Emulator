// Server Emulator.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h> //Windows Socket API 
#include <windows.h>  //Windows General Purpose API
#include <iostream>   //Console Output
#include "console.h"  //Custom console functions
#include <thread>  //C++11 standard of threading :)
#include <string>
#include <random>

#pragma comment(lib, "ws2_32.lib")

int startupServerForListening(unsigned short port);
std::string parseData(const char *packet);
bool run(int socket, std::string command);
int openChest(std::string com, size_t pos);
bool buyItem(std::string com, size_t pos);
bool connectionManager(int socket);



int main(int argc, char* argv[])
{
	int error;
	WSAData wsaData;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR) {
		pfail("Winsock Couldn´t start\n");
		return 1;
	}
	else{ std::cout << "Winsock Started\n"; }


	int serverSocket;
	serverSocket = startupServerForListening(47799);

	if (serverSocket == -1)
	{
		pfail("Network Startup Failed!");        // Check for errors
		pfail("Program Terminating");
		return 1;
	}


	int clientSocket;
	for (;;)
	{ 

	clientSocket = accept(serverSocket, 0, 0);
	// Check for errors
	if (clientSocket == SOCKET_ERROR)
	{
		pfail("Accept Failed!");
	}
	else
	{
		std::cout << "New connection done." << std::endl;
		std::thread p(connectionManager, (int)clientSocket);
		p.detach();
	}

	}
	


	std::cin.get();
	WSACleanup();
	return 0;
}


int openChest(std::string com, size_t pos)
{
	std::cout << "OpenChest found in: " << pos << '\n';
	std::string result = com.erase(0, 10);
	std::cout << "Chest Number: " << result;
	std::minstd_rand(1);
	return (rand() % 100);
	//return (std::stoi(result));
}

bool buyItem(std::string com, size_t pos)
{
	int itemPrice = 300;
	std::cout << "BuyItem found in: " << pos << '\n';
	std::string result = com.erase(0, 8);
	std::cout << "Money: " << result;

	if (std::stoi(result) >= itemPrice){ return TRUE; }
	else{ return FALSE; }
}

bool connectionManager(int socket)
{
	// The number of bytes I send/read ... will also serve as my error code
	int nBytes;
	//Buffer where we will store received data. it's filled with trash 
	char buffer[32];
	memset(buffer, 0xcc, 32);
	for (;;)
	{	
		
		nBytes = recv(socket, (char*)&buffer, sizeof(buffer), 0);
		switch (nBytes)
		{
		case 0:
			pfail("Can't receive: Closed connection.");
			return -1;
		case SOCKET_ERROR:
			pfail("Can't receive: Unknown error!");
			return -1;
		default:
			std::cout << "Received: " << nBytes << " bytes." << std::endl;
		}

		std::cout << "Received: " << buffer << std::endl << std::endl;
		run(socket, buffer);


	}
}

int startupServerForListening(unsigned short port)
{
	
	if (port==0 || port >=65535){
	pfail("Wrong Port");
	return -1;
	}
	
	// Create my socket
	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Make sure nothing bad happened
	if (mySocket == SOCKET_ERROR)
	{
		pfail("Error Opening Socket");
		return -1;
	}

	// The address structure
	struct sockaddr_in server;

	// Fill the address structure with appropriate data
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	// And now bind my socket
	if (bind(mySocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		pfail("Bind Failed!");
		closesocket(mySocket);
		Sleep(5000);
		return -1;

	}

	// Mark my socket for listening
	if (listen(mySocket, 5) == SOCKET_ERROR)
	{
		pfail("Listen Failed!");
		closesocket(mySocket);
		Sleep(5000);
		return -1;

	}

	printf("[*]Server Started\n");

	return mySocket;
}

bool run(int socket,std::string command)
{
	std::size_t tempPos;

	tempPos = command.find("OpenChest#");
	
	if (tempPos != std::string::npos)
	{
		int money;
		money = openChest(command,tempPos );
		std::cout << std::endl <<"Found " << money << "coins" << std::endl;
		//char packet[32] = "GetCoins#";
		std::string packet = "GetCoins#";
		std::string stringMoney;
		//char stringMoney[4];
		stringMoney = std::to_string(money);
		//_itoa(money, stringMoney, 10);
		//strcat(packet, stringMoney);
		packet += stringMoney;

		std::cout << "Ready to send!";
		send(socket, packet.c_str(), packet.length(), 0);
		std::cout << " DONE!" << std::endl;
		
	}
	else
	{
		tempPos = command.find("BuyItem#");
		if (tempPos != std::string::npos)
		{
			std::string buyOkpacket = "GetItem#1";
			std::string buyFailedPacket = "BuyDennied#0";
			//char buyOk[] = "GetItem#1";
			//char buyFl[] = "BuyDennied#0";
			bool x = buyItem(command, tempPos);

			if (x){ send(socket, buyOkpacket.c_str(), buyOkpacket.length(), 0); }
			else{ send(socket, buyFailedPacket.c_str(), buyFailedPacket.length(), 0); }
		}
	}
	

	//std::cout << command.erase(0,'Open');

	return 0;
}
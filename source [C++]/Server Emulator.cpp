// Server Emulator.cpp : Defines the entry point for the console application.
//
//Revision:2.0

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h> //Windows Socket API 
#include <windows.h>  //Windows General Purpose API
#include <iostream>   //Console Output
#include "console.h"  //Custom console functions
#include <thread>     //C++11 standard of threading :)
#include <string>     //C++   strings
#include <random>     //pseudo-random number generation

#pragma comment(lib, "ws2_32.lib") //Link a winsock related lib

/*Function prototypes*/

int startupServerForListening(unsigned short port); //Creates a server socket
bool run(int socket, std::string command);       //Analizes a command
int openChest(std::string &com, size_t pos);      //runs the 'openChest' command
bool buyItem(std::string &com, size_t pos);       //runs the 'buyItem' command
bool connectionManager(int socket);              //Entrypoint of every new thread, manages new connections



int main(int argc, char* argv[])
{
	int error;
	WSAData wsaData;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR) {
		pfail("Winsock Couldn´t start\n");
		return 1;
	}
	else{ std::cout << "Winsock Started\n"; }


	int serverSocket; //We'll create and bind a socket on port 47799
	serverSocket = startupServerForListening(47799); 

	if (serverSocket == -1)
	{
		pfail("Network Startup Failed!");     //Check for errors
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
	//If everything was fine, create a new thread to manage the new connection
	else
	{
		std::cout << "New connection done." << std::endl;
		//Thread entrypoint is 'connectionManager()' with argv 'clientSocket'
		std::thread p(connectionManager, (int)clientSocket);
		//Detachs the thread to make it free!
		p.detach();
	}

	}
	
	WSACleanup();
	return 0;
}

//Will parse a 'OpenChest' command
//The structure of packets is:
//OpenChest#XXXX...
//Where anything after # is the parameter (number of chests opened each time)
//The com variable is a reference so we're modifying the real value
int openChest(std::string &com, size_t pos)
{
	std::string result = com.substr(pos+10, 4); //OpenChest#XXXX -> The maximun number of chests opened at the same time is 9999
	std::cout << "Someone opened: " << result << " chests!" << std::endl;
	
	return (rand() % 100);
	//return (std::stoi(result));
}

//Will parse a 'buyItem' command
//The structure of packets is:
//BuyItem#XXXX...
//Where anything after # is the parameter (money of the client)
bool buyItem(std::string &com, size_t pos)
{
	//This is the price of the item in our server
	int itemPrice = 300;
	std::string result = com.erase(0, 8);
	std::cout << "Client with " << result << "coins requested a buy" << std::endl;

	if (std::stoi(result) >= itemPrice){ std::cout << "Buy Accepted" << std::endl; return TRUE; }
	else{ std::cout << "Buy Denied" << std::endl; return FALSE; }
}

//This function manages connections and is the 
//entrypoint for any new connection made by the main thread
bool connectionManager(int socket)
{
	// The number of bytes I send/read ... will also serve as my error code
	int nBytes;
	//Buffer where we will store received data. it's firstly filled with trash 
	std::string buffer(32, 0xcc);
	for (;;)
	{	
		/*Call the recv function, will receive a 32 bytes package
		even not the 32 bytes may be legit data (extra bytes are handled by recv() as trash)*/
		nBytes = recv(socket, (char*)&buffer[0], 32, 0);

		
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

//Based on Kaylires function
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

//Parses packets to detect the command sent by the client
bool run(int socket,std::string command)
{
	//A size_t variable to hold the position in a string where the important data is
	std::size_t tempPos;
	//We'll start searching the OpenChest command, if not found, search the next command (is not a really good code, but does its job)
	//Finds position where substring 'OpenChest#' starts
	tempPos = command.find("OpenChest#");
	
	//If there weren't any error and it was found
	if (tempPos != std::string::npos)
	{
		//int variable to hold the parsed clients money
		int money;
		money = openChest(command,tempPos );
		std::cout << std::endl <<"Found " << money << "coins" << std::endl;

		//We have the money stored into a int variable. We need to cast it to string
		std::string strMoney = std::to_string(money);
		//And build the packet
		std::string packet = "GetCoins#" + strMoney;

		//Send the crafted packet
		send(socket, packet.c_str(), packet.length(), 0);
		
		
	}
	else
	{
		//Finds the other command
		tempPos = command.find("BuyItem#");
		if (tempPos != std::string::npos)
		{
			//This is the packet we'll send if the client is rich!
			std::string buyOkpacket = "GetItem#1";
			//And this one if it's poor :(
			std::string buyFailedPacket = "BuyDennied#0";

			bool x = buyItem(command, tempPos);

			if (x){ send(socket, buyOkpacket.c_str(), buyOkpacket.length(), 0); }
			else{ send(socket, buyFailedPacket.c_str(), buyFailedPacket.length(), 0); }
		}
	}
	


	return 0;
}
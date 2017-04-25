// mainClient.cpp
//   This function serves as the "main" function for the client side
#include "TicTacToe.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int mainClient(int argc, char *argv[], std::string playerName)
{
	std::string host;
	std::string port;
	ServerStruct server[MAX_SERVERS];

	SOCKET s = connectsock("", "", "udp");	// Create a socket  (Don't need to designate a host or port for UDP)


	char broadcastAddress[v4AddressSize];
	strcpy_s(broadcastAddress, getBroadcastAddress());

	bool stillConnecting = true;
	while (stillConnecting)
	{
		// Find all Nim servers on our subnet
		std::cout << std::endl << "Looking for Nim servers ... " << std::endl;

		int numServers = getServers(s, broadcastAddress, TicTacToe_UDPPORT, server);

		if (numServers == 0) {
			std::cout << std::endl << "Sorry.  No Nim servers were found.  Try again later." << std::endl << std::endl;
		}
		else {
			// Display the list of servers found
			std::cout << std::endl << "Found Nim server";
			if (numServers == 1) {
				std::cout << ":  " << server[0].name << std::endl;
			}
			else {
				std::cout << "s:" << std::endl;
				for (int i = 0; i < numServers; i++) {
					std::cout << "  " << i + 1 << " - " << server[i].name << std::endl;
				}
				std::cout << std::endl << "  " << numServers + 1 << " - QUIT" << std::endl;
			}
			std::cout << std::endl;

			// Allow user to select someone to challenge
			int answer = 0;
			std::string answer_str;
			if (numServers == 1) {
				std::cout << "Do you want to play with " << server[0].name << "? ";
				std::getline(std::cin, answer_str);
				if (answer_str[0] == 'y' || answer_str[0] == 'Y') answer = 1;
			}
			else if (numServers > 1) {
				std::cout << "Who would you like to play with (1-" << numServers + 1 << ")? ";
				std::getline(std::cin, answer_str);
				answer = atoi(answer_str.c_str());
				if (answer > numServers) answer = 0;
			}

			if (answer >= 1 && answer <= numServers) {
				// Extract the opponent's info from the server[] array
				std::string serverName;
				serverName = server[answer - 1].name;		// Adjust for 0-based array
				host = server[answer - 1].host;
				port = server[answer - 1].port;

				// Append playerName to the TicTacToe_CHALLENGE string & send a challenge to host:port
				char buf[MAX_SEND_BUF];
				strcpy_s(buf, TicTacToe_CHALLENGE);
				strcat_s(buf, playerName.c_str());
				int len = UDP_send(s, buf, strlen(buf) + 1, (char*)host.c_str(), (char*)port.c_str());

				//wait for a reply; either YES or NO
				int status = wait(s, 20, 0);
				bool response = false;

				if (status > 0) {
					char resp[MAX_RECV_BUF];
					int len2 = UDP_recv(s, resp, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());

					//if YES, send GREAT! and start the game
					if (std::string(resp) == "YES") {
						response = true;
						char greatbuf[MAX_SEND_BUF];
						strcpy_s(greatbuf, "GREAT!");
						int len = UDP_send(s, greatbuf, strlen(greatbuf) + 1, (char*)host.c_str(), (char*)port.c_str());

						stillConnecting = false;
						// Play the game.  You are the challenger player
						int winner = Chat(s, serverName, host, port, serverName);
					}
					else {
						std::cout << "Request Denied. Please select another server or quit" << std::endl;
					}
				}
				else if (response == false) {
					//If no response is received from the other user within the time allotted 
					//(or the response is something other than “YES” or “NO”), 
					//your client code should assume the answer is “NO”; 
					//and allow your local user to either challenge someone else or exit.					std::cout << "Request Denied. Please select another server or quit" << std::endl;
				}
			}
			else if (answer > numServers) {
				std::cout << "please choose a valid server" << std::endl;
			}
			else {
				stillConnecting = false;
			}
		}
	}
	closesocket(s);
	return 0;
}
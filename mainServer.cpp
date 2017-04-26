// mainServer.cpp
//   This function serves as the "main" function for the server side
#include "TicTacToe.h"
#include <iostream>
#include <string>
#include <WinSock2.h>


int mainServer(int argc, char *argv[], std::string playerName)
{
	SOCKET s;
	char buf[MAX_RECV_BUF];
	std::string host;
	std::string port;
	char response_str[MAX_SEND_BUF];
	
	s = passivesock(TicTacToe_UDPPORT,"udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	int len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str() , (char*)port.c_str());

	bool finished = false;
	while (!finished) {
		if ( strcmp(buf,TicTacToe_QUERY) == 0) {
			// Respond to name query
			strcpy_s(response_str,TicTacToe_NAME);
			strcat_s(response_str,playerName.c_str());
			UDP_send(s, response_str, strlen(response_str)+1, (char*)host.c_str(), (char*)port.c_str());

		} else if ( strncmp(buf,TicTacToe_CHALLENGE,strlen(TicTacToe_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(buf,TicTacToe_CHALLENGE);
			if (startOfName != NULL) {
				//Give your user a chance to accept the challenge
				std::cout << std::endl << "Enter y to accept " << startOfName + strlen(TicTacToe_CHALLENGE) << "'s challenge";
				std::string answer;
				std::getline(std::cin, answer);
				if (answer[0] == 'y' || answer[0] == 'Y') {
					//If they do accept the challenge, your server code should send a UDP datagram back to
					//the client that contains the string "YES"
					char yesbuf[MAX_SEND_BUF];
					strcpy_s(yesbuf, "YES");
					int len = UDP_send(s, yesbuf, strlen(yesbuf) + 1, (char*)host.c_str(), (char*)port.c_str());

					// then wait (for up to 2 seconds) for a reply UDP-datagram from the client that 
					//contains the string "GREAT!"
					int status = wait(s, 2, 0);
					if (status > 0) {
						char resp[MAX_RECV_BUF];
						int len2 = UDP_recv(s, resp, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());

						if (std::string(resp) == "GREAT!") {
							std::string otherName(startOfName + strlen(TicTacToe_CHALLENGE));

							//choose initial board configuration
							//The server / host always specifies the number of rock piles 
							//and how many rocks are in each pile
							NimGame game;
							game.chooseConfig();

							// The specific rock pile configuration should be sent to the client 
							// machine in a datagram that has the format : “mn1n1n2n2n3n3nmnm”, 
							// where m is the number of piles and nini is a two digit number that 
							// represents the number of rocks in the ith pile. 
							char configbuf[MAX_SEND_BUF];
							char rockbuf[MAX_SEND_BUF];
							itoa(game.row_count, configbuf, 10);
							for (int i = 0; i < game.row_count; i++) {
								//If the number of rocks in the ith pile is less than 10,
								if (game.rows[i] < 10) {
									//the corresponding 2 digit number must have a leading zero.
									itoa(0, rockbuf, 10);
									strcat(configbuf, rockbuf);
								}
								itoa(game.rows[i], rockbuf, 10);
								strcat(configbuf, rockbuf);
							}

							//send the board config to the client
							int configLen = UDP_send(s, configbuf, strlen(configbuf) + 1, (char*)host.c_str(), (char*)port.c_str());

							// Play the game.  You are the host player
							int winner = Nim(s, (char*)playerName.c_str(), (char*)host.c_str(), (char*)port.c_str(), otherName, game, false);
							finished = true;
						}
					}					
				}
				else {
					//If they do not accept the challenge, send a UDP datagram back to the client containing the string “NO”
					char nobuf[MAX_SEND_BUF];
					strcpy_s(nobuf, "NO");
					int len = UDP_send(s, nobuf, strlen(nobuf) + 1, (char*)host.c_str(), (char*)port.c_str());
				}
			}
		}

		if (!finished) {
			char previous_buf[MAX_RECV_BUF];		strcpy_s(previous_buf,buf);
			std::string previous_host;				previous_host = host;
			std::string previous_port;				previous_port = port;

			// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
			bool newDatagram = false;
			int status = wait(s,1,0);	// We'll wait a second to see if we receive another datagram
			while (!newDatagram && status > 0) {
				len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
				if (strcmp(buf,previous_buf)==0 &&		// If this datagram is identical to previous one, ignore it.
					host == previous_host && 
					port == previous_port) {
						status = wait(s,1,0);			// Wait another second (still more copies?)
				} else {
					newDatagram = true;		// if not identical to previous one, keep it!
				}
			}

			// If we waited one (or more seconds) and received no new datagrams, wait for one now.
			if (!newDatagram ) {
				len = UDP_recv(s, buf, MAX_RECV_BUF, (char*)host.c_str(), (char*)port.c_str());
			}
		}
	}
	closesocket(s);

	return 0;
}
// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "TicTacToe.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cctype>
#include <regex>

int Nim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, std::string name, NimGame game, bool aiPlayer, bool moveDef)
{
	bool myMove = moveDef;
	std::string temp;
	std::string message;
	do {
		if (myMove) {
			//show board configuration
			game.draw_game();

			//make it so they can continually chat but once they make a move 
			//or forfeit their turn is over
			bool chatting = true;
			while (chatting)
			{
				if (!aiPlayer) 
				{
					bool validmessage = true;
					std::cout << "Input move (1-" << game.row_count << "), chat message, or forfeit (f): ";
					getline(std::cin, message);

					temp = message;

					for (char c : message) {
						c = tolower(c);
					}

					if (_stricmp(temp.c_str(), "f") == 0) {
						std::cout << "You have forfeited. You lose." << std::endl;
						//need to send a capital f to count as a forfeit
						message = "F";
						chatting = false;
					}
					else {
						try {
							//else if a #
							int rownum = stoi(message, nullptr);
							if (rownum <= game.row_count && rownum > 0) {
								std::cout << "how many would you like to remove? (1-" << game.rows[rownum - 1] << ")" << std::endl;
								std::string rockstr;
								std::getline(cin, rockstr);
								int rocknum = stoi(rockstr, nullptr);
								bool valid = game.remove_elements(rownum - 1, rocknum);
								if (valid) {
									//if 1 <= n <= numRocks, set message to move in right format
									//mnn (1st n is a 0 if n < 10)
									//set it so they can't make any more comments
									chatting = false;
									char messagebuf[MAX_SEND_BUF];
									char rockbuf[MAX_SEND_BUF];
									_itoa_s(rownum, messagebuf, 10);
									if (rocknum < 10) {
										_itoa_s(0, rockbuf, 10);
										strcat_s(messagebuf, rockbuf);
									}
									_itoa_s(rocknum, rockbuf, 10);
									strcat_s(messagebuf, rockbuf);
									message = messagebuf;
								}
								else {
									//else it was an invalid move
									//if user enters invalid move, let them choose again
									std::cout << "Please check your input and try again." << std::endl;
									validmessage = false;
								}
							}
							else {
								std::cout << "please enter a valid row" << std::endl;
								validmessage = false;
							}

						}
						catch (...) {
							//it's a comment, add a 'C' to the beginning to specify a chat
							message = "C" + temp;
						}
					}
					if (validmessage) {
						int len1 = UDP_send(s, (char*)message.c_str(), message.length() + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
					}
				}
				else
				{
					std::string aiMessage = game.computer();
					chatting = false;
					int len1 = UDP_send(s,(char*)aiMessage.c_str(), aiMessage.length() + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
				
				}
			}

			//check for win, if win show user and stop playing
			if (game.sum() == 0) {
				cout << "You win." << endl;
				return 1;
			}
			if (_stricmp(temp.c_str(), "f") == 0)
				return ABORT;
		}
		else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			char recv[MAX_RECV_BUF];
			bool chatting = true;
			while (chatting) {
				int status = wait(s, WAIT_TIME, 0);
				if (status > 0) {
					int len2 = UDP_recv(s, recv, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
					std::string check(recv);
					/*for (char c : check) {
						c = tolower(c);
					}*/

					if (_stricmp(check.c_str(), "f") == 0) {
						std::cout << "Your opponent has forfeited! Congratulations! You win!" << std::endl;
						chatting = false;
						return 1;
					}
					else if (check[0]=='C') {
						//if comment, display (and keep listening until get a move or forfeit)
						//get rid of c in front of comment
						std::string comment;
						comment = recv;
						comment.erase(0, 1);
						std::cout << name << ": " << recv << std::endl;
					}
					else if(std::regex_match(check.begin(), check.begin()+1, std::regex("[1-9]"))){
						//if move, check for validity then a win, stop chatting
						int row = check[0]-48;
						int rocks1 = check[1]-48;
						int rocks2 = check[2]-48;
						rocks1 *= 10;
						int rocks = rocks1 + rocks2;
						int valid = game.remove_elements(row - 1, rocks);
						if (!valid) {
							std::cout << "You have won by default (invalid move)" << std::endl;
							return 1;
						}
						else {
							chatting = false;
							std::cout << name << " removed " << rocks << " from pile " << row << endl;
							//if a win, show the user they lost and stop playing
							if (game.sum() == 0) {
								cout << "You lose." << endl;
								return 0;
							}
						}
					}
					else {
						//if anything else (doesn't start w/ 1-9, C, or F)
						//game over, show won by default, stop playing
						chatting = false;
						std::cout << "You have won by default (wrong format: " << recv << std::endl;
						return 1;
					}
					message = recv;
				}
				else {
					//game over, show won by default, stop playing
					std::cout << "You have won by default (timeout)" << std::endl;
					chatting = false;
					return 1;
				}
			}
		}
		myMove = !myMove;

		temp = message;

		for (char c : message) {
			c = tolower(c);
		}
	} while (message != "quit");

	return 0;
}
// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "TicTacToe.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cctype>

int Chat(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, std::string name, bool moveDef)
{
	bool myMove = moveDef;
	std::string temp;
	std::string message;
	do {
		if (myMove) {
			//show board configuration

			//make it so they can continually chat but once they make a move 
			//or forfeit their turn is over

			std::cout << "Input move (1-numPiles), chat message, or forfeit (f): "; 
			getline(std::cin, message);

			temp = message;

			for (char c : message) {
				c = tolower(c);
			}

			if (_stricmp(temp.c_str(), "f") == 0) {
				std::cout << "You have forfeited. You lose." << std::endl;
				//need to send a capital f to count as a forfeit
				for (char c : message) {
					c = toupper(c);
				}
			}
			//else if a #
				//if 1<=m<=numPiles ask how many rocks to remove
				//if 1<=n<=numRocks, set message to move in right format
				//mnn (1st n is a 0 if n<10)
				//set it so they can't make any more comments
				//check for win, if win show user and stop playing
				//if user enters invalid move, let them choose again
			else
				//it's a comment, add a 'C' to the beginning to specify a chat
				message = temp;

			int len1 = UDP_send(s, (char*)message.c_str(), message.length() + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
			if (_stricmp(temp.c_str(), "f") == 0)
				return ABORT;
		}
		else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			int status = wait(s, WAIT_TIME, 0);
			if (status > 0) {
				char moveRecv[MAX_RECV_BUF];
				int len2 = UDP_recv(s, moveRecv, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
				std::string check(moveRecv);
				for (char c : check) {
					c = tolower(c);
				}
				if (_stricmp(check.c_str(), "f") == 0) {
					std::cout << "Your opponent has forfeited! Congratulations! You win!" << std::endl;
					return ABORT;
				}
				//check to see if move or comment
				//if comment, show (and keep listening until get a move or forfeit)
				//if move, check for validity then a win
					//update the board if valid and not a win
					//if a win, show the user they lost and stop playing
				//if anything else (doesn't start w/ 1-9, C, or F)
					//game over, show won by default, stop playing
				std::cout << name << ": " << moveRecv << std::endl;
				message = moveRecv;
			}
			else {
				//game over, win by default, stop playing
				return ABORT;
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
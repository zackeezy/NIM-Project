// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "TicTacToe.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cctype>

int Nim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, std::string name, NimGame game, bool moveDef)
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
				std::cout << "Input move (1-" << game.row_count << "), chat message (c), or forfeit (f): ";
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
				//else if () {
					//else if a #
					//if 1<=m<=numPiles ask how many rocks to remove
					//if 1<=n<=numRocks, set message to move in right format
					//mnn (1st n is a 0 if n<10)
					//set it so they can't make any more comments
					//check for win, if win show user and stop playing
					//if user enters invalid move, let them choose again

				//}
			
				else {
					//it's a comment, add a 'C' to the beginning to specify a chat
					message = "C" + temp;
				}
				int len1 = UDP_send(s, (char*)message.c_str(), message.length() + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
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
					for (char c : check) {
						c = tolower(c);
					}
					if (_stricmp(check.c_str(), "f") == 0) {
						std::cout << "Your opponent has forfeited! Congratulations! You win!" << std::endl;
						chatting = false;
						return ABORT;
					}
					//check to see if move or comment
					//if comment, display (and keep listening until get a move or forfeit)
						std::cout << name << ": " << recv << std::endl;
					//if move, check for validity then a win, stop chatting
					//update the board if valid and not a win
					//if a win, show the user they lost and stop playing
					//if anything else (doesn't start w/ 1-9, C, or F)
					//game over, show won by default, stop playing
					message = recv;
				}
				else {
					//game over, show won by default, stop playing
					chatting = false;
					return ABORT;
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
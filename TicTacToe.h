#pragma once

#include <WinSock2.h>
#include <string>
#include "gamelogic.cpp"


static char TicTacToe_UDPPORT[] = "29333";			// Port number used by Chat servers

#define TicTacToe_QUERY  "Who?"
#define TicTacToe_NAME	 "Name="
#define TicTacToe_CHALLENGE "Player="

const int v4AddressSize		= 16;
const int MAX_INTERFACES	= 20;
const int WAIT_TIME			= 60;
const int MAX_SEND_BUF		= 2048;
const int MAX_RECV_BUF		= 2048;
const int MAX_SERVERS		= 100;
const int X_PLAYER			= 1;
const int O_PLAYER			= 2;
const int ABORT				= -1;
const int noWinner			= 0;
const int xWinner			= 1;
const int oWinner			= 2;
const int TIE				= 3;

static char board[10];

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

SOCKET connectsock (char*, char*, char*);
SOCKET passivesock (char*, char*);
int UDP_recv (SOCKET, char*, int, char*, char*);
int UDP_send (SOCKET, char*, int, char*, char*);
int wait(SOCKET, int, int);
char* getBroadcastAddress();
int getServers(SOCKET, char*, char*, ServerStruct[]);
int Nim(SOCKET, std::string, std::string, std::string, std::string, NimGame game, bool moveDef = true);
int mainServer(int, char*[], std::string);
int mainClient(int, char*[], std::string);

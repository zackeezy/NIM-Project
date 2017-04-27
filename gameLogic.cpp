#include <iostream>
#include <vector>
#include <string>
//#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using std::vector;
using std::cout;
using std::cin;
using std::endl;

class NimGame {

public:
	vector<int> rows;
	//This number is specified by the host.
	int row_count = 4;
	/*
	This function uses the vector and the global(now class) value rows
	Iterates thru the vector and adds the number of rocks in total
	returns the sum of the rocks
	(to check to see if the game is over, i.e. 0 rocks are left)
	*/
	int sum() {
		int value = 0;
		for (unsigned int i = 0; i<rows.size(); i++) {
			value += rows[i];
		}
		return value;
	}

	/*
	This function will draw to console the rows and how many rocks are per row
	this uses the global variables so it does not need to take parameters as is
	*/
	void draw_game() {
		for (int i = 0; i < rows.size(); i++) {
			cout << i + 1 << ": (" << rows[i]<<")";
			for (int j = 0; j < rows[i]; j++) {
				cout << " * ";
			}
			cout << endl;
		}
	}

	/*
	This function will display a cout statement and return the value given by user
	*/
	void input(int &command, const char* command_name) {
		cout << "Press " << command_name << ". Use 0 to exit." << endl;
		cin >> command;
	}

	/*This function will take:
	1- Row number: Describes the row where we want to extract rocks from
	2- number: The amount of rocks we want to take from that rock
	Return: Bool describing is the subtraction was sucessful
	*/
	bool remove_elements(int row, int number) {

		if (row < rows.size()) {
			if (number <= rows[row] && number != 0) {
				rows[row] -= number;
				return true;
			}
		}
		return false;
	}

	/*
	This is a little AI model I found online, so now you can play the nim game with yourself
	*/
	int xOr() {
		int value = 0;
		for (unsigned int i = 0; i<rows.size(); i++) {
			value ^= rows[i];
		}
		return value;
	}

	void add_element(int row, int number) {
		rows[row] += number;
	}

	std::string computer() {
		//cout << "Computer turn..." << endl;
		int sum = xOr();
		for (int i = 0; i < rows.size(); i++) {
			if (rows[i] > 0) {
				for (int j = 1; j <= rows[i]; j++) {
					remove_elements(i, j);
					sum = xOr();
					if (sum != 0) {
						add_element(i, j);
					}
					else {
						cout << "Removed " << j << " from row " << i + 1 << endl;
						std::string aiMessage;
						//Doesn't recognize MAX_SEND_BUF for some reason.
						char message[100];
						char rocks[10];
						char pile[10];

						_itoa_s((i + 1), pile, 10);
						strcpy_s(message, pile);

						if (j < 10)
						{
							_itoa_s(0, rocks, 10);
							strcat_s(message, rocks);
						}
						_itoa_s(j, rocks, 10);
						strcat_s(message, rocks);
					
						aiMessage = message;

						return aiMessage;
					}
				}
			}
		}
		std::string out;
		srand(time(NULL));
		int x = rand() % 9;
		int y = rand() % (rows[x]);
		out.append(x);
		if (y < 10) {
			out.append(0);
			out.append(y);
		}
		else {
			out.append(y);
		}
		return out;

	}
	//END of AI

	//parse through the received string and get the initial game configuration
	void parseString(char* received_string) {
		row_count = received_string[0] - '0';

		rsize_t length = strlen(received_string) - 1;
		//Max number of piles is two
		int rows_received[9];

		//Getting the size of all the rows
		int index = 0;
		for (int i = 0; i < length / 2; i++) {
			rows_received[i] = ((received_string[index + 1] - '0') * 10) + (received_string[index + 2] - '0');
			index += 2;
		}
		//Populating the vector
		for (int i = 0; i < row_count; i++) {
			rows.push_back(rows_received[i]);
		}
	}

	//choose initial board configuration
	//The server / host always specifies the number of rock piles 
	//and how many rocks are in each pile (3 <= Piles <= 9; 1 <= Rocks per Pile <= 20)
	void chooseConfig() {
		//std::default_random_engine generator;
		//std::uniform_int_distribution<int> rowcount_distribution(3, 9);
		//row_count = rowcount_distribution(generator);  // generates number in the range 3..9
		//std::uniform_int_distribution<int> rockcount_distribution(1, 20);
		row_count = rand() % 9 + 3;
		srand(time(NULL));
		for (int i = 0; i < row_count; i++) {
			//rows.push_back(rockcount_distribution(generator));  // generates number in the range 1..20
			rows.push_back(rand() % 20 + 1);
		}
	}

	int playgame() {

		//Populating the vector:
		//Sample String "4 10 02 10 14" 4 rows, 1-) 10 || 2-) 02 || 3-) 10 || 4-) 14
		//NOTE: The requirements say that string must be 19 length the most, when receiveing the data please check for that to be true
		char received_string[] = "410021014";

		parseString(received_string);

		cout << "Game Prototype" << endl;

		int command = -1;
		bool validated = false;
		while (command != 0 || !validated) {

			draw_game();
			input(command, "row number");
			if (command != 0) {
				int row = command;
				input(command, "how much you want to remove");
				if (command != 0) {
					validated = remove_elements(row - 1, command);
					//Check to see is game is over
					if (sum() == 0) {
						cout << "You win." << endl;
						command = 0;
						break;
					}
					else {
						//Validate will be true if a valid turn just passed computer will make its move
						if (validated) computer();
						//Check to see if game is over
						if (sum() == 0) {
							cout << "You loose." << endl;
							command = 0;
						}
					}
				}
			}
			else {
				return 0;
			}
		}
		return 0;
	}
};

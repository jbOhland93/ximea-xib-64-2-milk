#include "../headers/UserInputHandler.h"

#include <ncurses.h>
#include <cstring>

using namespace std;

vector<userCmd> UserInputHandler::cmdList = {
	CMD_HELP,
	CMD_QUIT,
	CMD_NONEXISTENT
};

map<string,userCmd> UserInputHandler::cmdStrings = {
	{"help", CMD_HELP},
	{"quit", CMD_QUIT},
	{"nonexist", CMD_NONEXISTENT},
};

map<userCmd,string> UserInputHandler::cmdHelp = {
	{CMD_HELP, "Shows a list of available commands"},
	{CMD_QUIT, "Quits the programm"},
	{CMD_NONEXISTENT, "Nonexistent command"}
};

make_pair

void UserInputHandler::core()
{
    initscr();
    addstr("\n\n === Welcome to our WIP CLI. ===\n");
    addstr("\nEnter any string and hit enter to display the response.\n");
    addstr("Enter \"quit\" to exit the CLI.\n");
    mvprintw(LINES-2,0,mPrompt);

    while (mRunning) {
        waitOnNextCmd();

        char str[1024];
        getstr(str);
		handleInput(str);
    }
    endwin();
}

void UserInputHandler::waitOnNextCmd()
{
	wmove(stdscr, LINES-2, strlen(mPrompt));
	clrtoeol();
}

void UserInputHandler::handleInput(char* input)
{
	if (cmdStrings.find(input) == cmdStrings.end()) {
		handleUnknownCmd(input);
	} else {
		switch(cmdStrings[input]) {
		case CMD_HELP:
			execCmdHelp();
			break;
		case CMD_QUIT:
			mRunning = false;
			break;
		default:
			mvprintw(LINES-1, 0, "Command found but no switch case implemented: ");
			clrtoeol();
			addstr(input);
		}
}

void UserInputHandler::handleUnknownCmd(char* input)
{
	mvprintw(LINES-1, 0, mAnswerUnknown);
	clrtoeol();
	addstr(input);
}

void UserInputHandler::execCmdHelp(char* input)
{
	int maxLen = 0;
	
	map<string, userCmd>::iterator it;
	for (it = cmdStrings.begin(); it != cmdStrings.end(); it++)
	{
		string cmdCall = it->first;
		userCmd cmd = it->second;
		
		// Do something ...
	}
}
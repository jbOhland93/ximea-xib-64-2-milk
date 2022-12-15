#include "../headers/UserInputHandler.h"

#include <ncurses.h>
#include <cstring>

using namespace std;

vector<userCmd> UserInputHandler::cmdList = {
	userCmd::CMD_HELP,
	userCmd::CMD_QUIT,
};

map<string,userCmd> UserInputHandler::cmdStrings = {
	{"help", userCmd::CMD_HELP},
	{"quit", userCmd::CMD_QUIT},
};

map<userCmd,string> UserInputHandler::cmdHelp = {
	{userCmd::CMD_HELP, "Shows a list of available commands"},
	{userCmd::CMD_QUIT, "Quits the programm"},
};

void UserInputHandler::core()
{
    initscr();
    wmove(stdscr, 0, 0);
    addstr("=== Welcome to our WIP CLI. ===\n");
    addstr("\nEnter any string and hit enter to display the response.\n");
    addstr("Enter \"quit\" to exit the CLI.\n");
    addstr("Enter \"help\" for a list of comand options.\n\n");
    mPrintareaStart = getcury(stdscr);

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
	wmove(stdscr, LINES-2, 0);
	clrtoeol();
    addstr(mPrompt);
}

void UserInputHandler::handleInput(char* input)
{
	if (cmdStrings.find(input) == cmdStrings.end()) {
		handleUnknownCmd(input);
	} else {
		switch(cmdStrings[input]) {
		case userCmd::CMD_HELP:
			execCmdHelp();
			break;
		case userCmd::CMD_QUIT:
			mRunning = false;
			break;
		default:
			mvprintw(LINES-1, 0, "Command found but no switch case implemented: ");
			clrtoeol();
			addstr(input);
		}
    }
}

void UserInputHandler::handleUnknownCmd(char* input)
{
	mvprintw(LINES-1, 0, mAnswerUnknown);
	clrtoeol();
	addstr(input);
}

void UserInputHandler::execCmdHelp()
{
    int descrCol = 32;

    clearPrintArea();
    addstr("The following commands are available:\n");

	map<string, userCmd>::iterator it;
	for (it = cmdStrings.begin(); it != cmdStrings.end(); it++)
	{
        addstr(it->first.c_str());
        wmove(stdscr, getcury(stdscr), descrCol);
        addstr(cmdHelp[it->second].c_str());
        addstr("\n");
	}
    clearResponseLine();
}

void UserInputHandler::clearPrintArea()
{
    wmove(stdscr, mPrintareaStart, 0);
    int curY = mPrintareaStart;
    while (curY < LINES-2) {
        clrtoeol();
        curY++;
        wmove(stdscr, curY, 0);
    }
    wmove(stdscr, mPrintareaStart, 0);
}

void UserInputHandler::clearResponseLine()
{
    wmove(stdscr, LINES-1, 0);
    clrtoeol();
}
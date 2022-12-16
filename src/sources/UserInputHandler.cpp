#include "../headers/UserInputHandler.h"

#include <ncurses.h>
#include <cstring>
#include "UserInputHandler.h"

using namespace std;

vector<userCmd> UserInputHandler::cmdList = {
	userCmd::CMD_HELP,
    userCmd::CMD_START_ACQ,
	userCmd::CMD_STOP_ACQ,
    userCmd::CMD_GET_FPS,
	userCmd::CMD_QUIT,
};

map<string,userCmd> UserInputHandler::cmdStrings = {
	{"help", userCmd::CMD_HELP},
    {"start", userCmd::CMD_START_ACQ},
    {"stop", userCmd::CMD_STOP_ACQ},
    {"fps", userCmd::CMD_GET_FPS},
	{"quit", userCmd::CMD_QUIT},
};

map<userCmd,string> UserInputHandler::cmdHelp = {
	{userCmd::CMD_HELP, "Shows a list of available commands"},
    {userCmd::CMD_START_ACQ, "Starts the camera acquisition"},
    {userCmd::CMD_STOP_ACQ, "Stops the camera acquisition"},
    {userCmd::CMD_GET_FPS, "Prints the current estimated framerate"},
	{userCmd::CMD_QUIT, "Quits the programm"},
};

UserInputHandler::UserInputHandler(std::shared_ptr<Acquisitor> acq)
    : mp_acquisitor(acq)
{
}

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
        case userCmd::CMD_START_ACQ:
			execCmdStartStopAcq(true);
			break;
        case userCmd::CMD_STOP_ACQ:
			execCmdStartStopAcq(false);
			break;
        case userCmd::CMD_GET_FPS:
            execCmdPrintFPS();
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
    clearResponseLine();
	addstr(mAnswerUnknown);
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

void UserInputHandler::execCmdStartStopAcq(bool start)
{
    clearResponseLine();
    if (start)
    {
        if (mp_acquisitor->isAcquiring())
            mvprintw(LINES-1, 0, "Acquisition is already running (%d frames acquired).", mp_acquisitor->getFrameCount());
        else
        {
            int w,h;
            getmaxyx(stdscr, h, w);
            
            WINDOW* win = newwin(0,0,w,h);
            overwrite(stdscr, win);

            mp_acquisitor->startAcquisition();
            overwrite(win, stdscr);
            addstr("Acquisition started!");
        }
    }
    else
        if (mp_acquisitor->isAcquiring())
        {
            mp_acquisitor->stopAcquisition();
            mvprintw(LINES-1, 0, "Acquisition stopped. %d frames acquired in total.", mp_acquisitor->getFrameCount());
        }
        else
        {
            addstr("Acquisition not running, nothing to be done.");
        }
}

void UserInputHandler::execCmdPrintFPS()
{
    clearResponseLine();
    mvprintw(LINES-1, 0, "The current framerate is approximately %.3f frames per second.", mp_acquisitor->getFPS());
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
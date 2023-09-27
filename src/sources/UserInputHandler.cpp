#include "UserInputHandler.h"

#include <ncurses.h>
//#include <cstring>
#include <sstream>
#include <iterator>

#include "UserInputHandler.h"

//#include <algorithm>
//#include <iterator>

using namespace std;

vector<userCmd> UserInputHandler::cmdList = {
	userCmd::CMD_HELP,
    userCmd::CMD_START_ACQ,
	userCmd::CMD_STOP_ACQ,
    userCmd::CMD_GET_FPS,
    userCmd::CMD_SET_EXPOSURE,
    userCmd::CMD_SET_FPS,
    userCmd::CMD_SET_ROI,
    userCmd::CMD_RESIZE_ROI,
	userCmd::CMD_QUIT,
};

map<string,userCmd> UserInputHandler::cmdStrings = {
	{"help", userCmd::CMD_HELP},
    {"start", userCmd::CMD_START_ACQ},
    {"stop", userCmd::CMD_STOP_ACQ},
    {"fps", userCmd::CMD_GET_FPS},
    {"setExpos", userCmd::CMD_SET_EXPOSURE},
    {"setFPS", userCmd::CMD_SET_FPS},
    {"setROI", userCmd::CMD_SET_ROI},
    {"resize", userCmd::CMD_RESIZE_ROI},
	{"quit", userCmd::CMD_QUIT},
};

map<userCmd,string> UserInputHandler::cmdHelp = {
	{userCmd::CMD_HELP, "Shows a list of available commands"},
    {userCmd::CMD_START_ACQ, "Starts the camera acquisition"},
    {userCmd::CMD_STOP_ACQ, "Stops the camera acquisition"},
    {userCmd::CMD_GET_FPS, "Prints the current estimated framerate"},
    {userCmd::CMD_SET_EXPOSURE, "Sets the exposure time in microseconds"},
    {userCmd::CMD_SET_FPS, "Sets the framerate limit in frames per second; -1 = freerun"},
    {userCmd::CMD_SET_ROI, "Sets the region of interest in pixels"},
    {userCmd::CMD_RESIZE_ROI, "Shifts a user defined border of the ROI by a chosen value"},
	{userCmd::CMD_QUIT, "Quits the programm"},
};

UserInputHandler::UserInputHandler(std::shared_ptr<Acquisitor> acq, std::shared_ptr<CamConfigurator> camConf)
    : mp_acquisitor(acq), mp_camConf(camConf)
{
}

void UserInputHandler::core()
{
    initscr();
    wmove(stdscr, 0, 0);
    addstr("=== Welcome to the CLI of the XIMEA-xiB-64-2-milk application. ===\n");
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
    
    // Cleanup
    if (mp_acquisitor->isAcquiring())
        mp_acquisitor->stopAcquisition();
}

void UserInputHandler::waitOnNextCmd()
{
	wmove(stdscr, LINES-2, 0);
	clrtoeol();
    addstr(mPrompt);
}

void UserInputHandler::handleInput(char* input)
{
    istringstream iss(input);
    vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
    if (tokens.size() == 0)
        return; // Empty command string - perhaps user pressed enter.
    string cmd = tokens.at(0);

	if (cmdStrings.find(cmd) == cmdStrings.end()) {
		handleUnknownCmd(tokens.at(0));
	} else {
		switch(cmdStrings[cmd]) {
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
        case userCmd::CMD_SET_EXPOSURE:
            execCmdSetExposure(tokens);
            break;
        case userCmd::CMD_SET_FPS:
            execCmdSetFPS(tokens);
            break;
        case userCmd::CMD_SET_ROI:
            execCmdSetROI(tokens);
            break;
        case userCmd::CMD_RESIZE_ROI:
            execCmdSetResizeROI(tokens);
            break;
		case userCmd::CMD_QUIT:
			mRunning = false;
			break;
		default:
			mvprintw(LINES-1, 0, "Command found but no switch case implemented: ");
			clrtoeol();
			addstr(cmd.c_str());
		}
    }
}

void UserInputHandler::handleUnknownCmd(string cmd)
{
    clearResponseLine();
	addstr(mAnswerUnknown);
	addstr(cmd.c_str());
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

void UserInputHandler::execCmdSetExposure(std::vector<std::string> args)
{
    clearResponseLine();
    if (args.size() < 2)
        mvprintw(LINES-1, 0, "Please add the desired exposure time in microseconds.");
    else
    {
        try 
        {
            float us = stof(args.at(1));
            if (us < 0)
            mvprintw(LINES-1, 0, "The exposure time has to be greater than 0.");
            else
            {
                float set = mp_camConf->setExposureTime_us(us);
                if (set == -1)
                {
                    string msg = mp_camConf->getErrorDescription();
                    mvprintw(LINES-1, 0, "The camera did not accept the given value of %.3f. It could be out of range. Error: %s", us, msg.c_str());
                    mp_camConf->clearError();
                }
                else
                    mvprintw(LINES-1, 0, "The exposure time has ben set to %.3f us.", set);
            }
        }
        catch (std::invalid_argument e)
        {
            mvprintw(LINES-1, 0, "The first parameter must be a numerical value.");
        }
    }
}

void UserInputHandler::execCmdSetFPS(std::vector<std::string> args)
{
    clearResponseLine();
    if (args.size() < 2)
        mvprintw(LINES-1, 0, "Please add the desired frame rate frames per second.");
    else
    {
        try 
        {
            float fps = stof(args.at(1));
            if (fps < 0)
            {
                bool freeRun = mp_camConf->setFreeRun();
                if (!freeRun)
                {
                    string msg = mp_camConf->getErrorDescription();
                    mvprintw(LINES-1, 0, "The camera did not switch to freerun mode. Error: %s", msg.c_str());
                    mp_camConf->clearError();
                }
                else
                    mvprintw(LINES-1, 0, "The camera has been set to freerun mode.");
            }
            else if (fps < 0.8)
                mvprintw(LINES-1, 0, "The minimum framerate is 0.8 Hz. Values smaller than 0 will trigger the free run mode");
            else
            {
                float setFPS = mp_camConf->setFrameRateLimit(fps);
                if (setFPS == -1)
                {
                    string msg = mp_camConf->getErrorDescription();
                    mvprintw(LINES-1, 0, "The camera did not accept the given frame rate limit of %.3f. It could be out of range. Error: %s", setFPS, msg.c_str());
                    mp_camConf->clearError();
                }
                else
                    mvprintw(LINES-1, 0, "The frame rate limit has ben set to %.3f Hz.", setFPS);
            }
        }
        catch (std::invalid_argument e)
        {
            mvprintw(LINES-1, 0, "The first parameter must be a numerical value.");
        }
    }
}

void UserInputHandler::execCmdSetROI(std::vector<std::string> args)
{
    clearResponseLine();
    if (args.size() < 4)
        mvprintw(LINES-1, 0, "Please add the width, height, X- and Y-offset.");
    else
    {
        try 
        {
            int width = stoi(args.at(1));
            int height = stoi(args.at(2));
            int offX = stoi(args.at(3));
            int offY = stoi(args.at(4));
            if (width < mp_camConf->getROIwidthIncrement())
                mvprintw(LINES-1, 0, "The width (1st arg) has to be at least %d.", mp_camConf->getROIwidthIncrement());
            else if (width > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The width (1st arg) has to be smaller than %d.", mp_camConf->getSensorWidth());
            else if (height < mp_camConf->getROIheihtIncrement())
                mvprintw(LINES-1, 0, "The height (2nd arg) has to be at least %d.", mp_camConf->getROIheihtIncrement());
            else if (height > mp_camConf->getSensorHeight())
                mvprintw(LINES-1, 0, "The height (2nd arg) has to be smaller than %d.", mp_camConf->getSensorHeight());
            else if (offX < 0)
                mvprintw(LINES-1, 0, "The X-offset (3rd arg) has to be greater than 0.");
            else if (offX + width > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The sum of the width (1st arg) and the X-offset (3rd arg) has to be less than %d.", mp_camConf->getSensorWidth());
            else if (offY < 0)
                mvprintw(LINES-1, 0, "The Y-offset (4th arg) has to be greater than 0.");
            else if (offY + height > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The sum of the height (2st arg) and the Y-offset (4th arg) has to be less than %d.", mp_camConf->getSensorHeight());
            else
            {
                mp_acquisitor->pauseAcquisition();
                shared_ptr<vector<int>> actualROI = mp_camConf->setROI(width, height, offX, offY);
                if (mp_camConf->error())
                {
                    string msg = mp_camConf->getErrorDescription();
                    mvprintw(LINES-1, 0, "The camera did not accept the given ROI. It could be out of range. Error: %s", msg.c_str());
                    mp_camConf->clearError();
                }
                else
                {
                    mp_acquisitor->resumeAcquisition();
                    mvprintw(LINES-1, 0, "The exposure ROI has ben set to w=%d, h=%d, offX=%d, offY=%d, payload=%d bytes.",
                        actualROI.get()->at(0),
                        actualROI.get()->at(1),
                        actualROI.get()->at(2),
                        actualROI.get()->at(3),
                        actualROI.get()->at(4));
                }
            }
        }
        catch (std::invalid_argument e)
        {
            mvprintw(LINES-1, 0, "All four parameters must be integer values.");
        }
    }
}

void UserInputHandler::execCmdSetResizeROI(vector<string> args)
{
    clearResponseLine();
    if (args.size() < 3)
        mvprintw(LINES-1, 0, "Please indicate the border (either '-l', '-t', '-r' or '-b'), as well as the offset value in pixels");
    else
    {
        try 
        {
            string direction = args.at(1);
            int increment = stoi(args.at(2));

            shared_ptr<vector<int>> newROI = mp_camConf->getROI();
            if (direction == "-l")
            {
                newROI.get()->at(0) -= increment;
                newROI.get()->at(2) += increment;
            }
            else if (direction == "-t")
            {
                newROI.get()->at(1) -= increment;
                newROI.get()->at(3) += increment;
            }
            else if (direction == "-r")
                newROI.get()->at(0) += increment;
            else if (direction == "-b")
                newROI.get()->at(1) += increment;
            else
            {
                mvprintw(LINES-1, 0, "Please indicate the border to shift (either '-l', '-t', '-r' or '-b').");
                return;
            }

            if (newROI.get()->at(0) < mp_camConf->getROIwidthIncrement())
                mvprintw(LINES-1, 0, "The width has to be at least %d.", mp_camConf->getROIwidthIncrement());
            else if (newROI.get()->at(0) > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The width has to be smaller than %d.", mp_camConf->getSensorWidth());
            else if (newROI.get()->at(1) < mp_camConf->getROIheihtIncrement())
                mvprintw(LINES-1, 0, "The height has to be at least %d.", mp_camConf->getROIheihtIncrement());
            else if (newROI.get()->at(1) > mp_camConf->getSensorHeight())
                mvprintw(LINES-1, 0, "The height has to be smaller than %d.", mp_camConf->getSensorHeight());
            else if (newROI.get()->at(2) < 0)
                mvprintw(LINES-1, 0, "The X-offset has to be greater than 0.");
            else if (newROI.get()->at(2) + newROI.get()->at(0) > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The sum of the width and the X-offset has to be less than %d.", mp_camConf->getSensorWidth());
            else if (newROI.get()->at(3) < 0)
                mvprintw(LINES-1, 0, "The Y-offset has to be greater than 0.");
            else if (newROI.get()->at(3) + newROI.get()->at(1) > mp_camConf->getSensorWidth())
                mvprintw(LINES-1, 0, "The sum of the height and the Y-offset has to be less than %d.", mp_camConf->getSensorHeight());
            else
            {
                mp_acquisitor->pauseAcquisition();
                shared_ptr<vector<int>> actualROI = mp_camConf->setROI(newROI.get()->at(0), newROI.get()->at(1), newROI.get()->at(2), newROI.get()->at(3));
                if (mp_camConf->error())
                {
                    string msg = mp_camConf->getErrorDescription();
                    mvprintw(LINES-1, 0, "The camera did not accept the given ROI. It could be out of range. Error: %s", msg.c_str());
                    mp_camConf->clearError();
                }
                else
                {
                    mp_acquisitor->resumeAcquisition();
                    mvprintw(LINES-1, 0, "The exposure ROI has ben set to w=%d, h=%d, offX=%d, offY=%d, payload=%d bytes.",
                        actualROI.get()->at(0),
                        actualROI.get()->at(1),
                        actualROI.get()->at(2),
                        actualROI.get()->at(3),
                        actualROI.get()->at(4));
                }
            }
        }
        catch (std::invalid_argument e)
        {
            mvprintw(LINES-1, 0, "The second argument must be an integer value.");
        }
    }
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
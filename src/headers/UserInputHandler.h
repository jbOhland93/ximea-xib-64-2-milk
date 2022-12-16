#ifndef USERINPUTHANDLER_H
#define USERINPUTHANDLER_H

#include <string>
#include <map>
#include <vector>

#include "Acquisitor.h"
#include "CamConfigurator.h"

enum class userCmd{
	CMD_HELP,
	CMD_START_ACQ,
	CMD_STOP_ACQ,
	CMD_GET_FPS,
	CMD_SET_EXPOSURE,
	CMD_QUIT
};

class UserInputHandler
{
    public:
		static std::vector<userCmd> cmdList;
		static std::map<std::string,userCmd> cmdStrings;
		static std::map<userCmd,std::string> cmdHelp;
		
		// Constructor
		UserInputHandler(std::shared_ptr<Acquisitor> acq, std::shared_ptr<CamConfigurator> mp_camConf);

        // The central loop of the input handler. Handles user input.
        void core();

    private:
		const std::shared_ptr<Acquisitor> mp_acquisitor;
		const std::shared_ptr<CamConfigurator> mp_camConf;
		bool mRunning = true;
		int mPrintareaStart;
		const char * mPrompt = "Enter command: ";
		const char * mAnswerUnknown = "Command not found: ";
        std::string mCurrentCommand;

		// Make default ctor private
		UserInputHandler();
		
		// Flushes the last user input and places the cursor at the cmd input position
        void waitOnNextCmd();
		// Takes the input buffer, parses for known commands and calls the corresponding methods
		void handleInput(char* input);
		
		// ==== CMD methods ====
		// Prints a response to the user, telling him that the requested command does not exist
		void handleUnknownCmd(std::string cmd);
		// Prints a help list
		void execCmdHelp();
		// Starts/Stops the acquisition
		void execCmdStartStopAcq(bool start);
		// Prints the current estimated framerate
		void execCmdPrintFPS();
		// Sets the exposure
		void execCmdSetExposure(std::vector<std::string> args);
		
		// === Print methods ===
		// Clears the print area
		void clearPrintArea();
		// Clears the line below the user input prompt
		void clearResponseLine();

};

#endif // USERINPUTHANDLER_H

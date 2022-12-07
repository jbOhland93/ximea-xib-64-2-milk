#ifndef USERINPUTHANDLER_H
#define USERINPUTHANDLER_H

#include <string>
#include <map>
#include <vector>

enum class userCmd{
	CMD_HELP,
	CMD_QUIT,
	CMD_NONEXISTENT
};

class UserInputHandler
{
    public:
		static std::vector<userCmd> cmdList;
		static std::map<std::string,userCmd> cmdStrings;
		static std::map<userCmd,std::string> cmdHelp;
		
        // The central loop of the input handler. Handles user input.
        void core();

    private:
		bool mRunning = true;
		const char * mPrompt = "Enter command: ";
		const char * mAnswer = "You entered: ";
		const char * mAnswerUnknown = "Command not found: ";
        std::string mCurrentCommand;
		
		// Flushes the last user input and places the cursor at the cmd input position
        void waitOnNextCmd();
		// Takes the input buffer, parses for known commands and calls the corresponding methods
		void handleInput(char* input);
		
		// ==== CMD methods ====
		// Prints a response to the user, telling him that the requested command does not exist
		void handleUnknownCmd(char* input);
		// Prints a help list
		void execCmdHelp();
		
};

#endif // USERINPUTHANDLER_H

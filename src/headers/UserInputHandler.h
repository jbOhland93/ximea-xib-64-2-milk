#ifndef USERINPUTHANDLER_H
#define USERINPUTHANDLER_H

#include <string>

class UserInputHandler
{
    public:
        // The central loop of the input handler. Handles user input.
        void core();

    private:
        std::string mCurrentCommand;
        char mCurrentInputChar;

        // Reads user input one by one after a keyboard hit is detected in order to offer non-bocking user input.
        void handleKeyboardInput();

        // Trims the preceeding and trailing whitespace of the given string.
        std::string trimWhitespace(std::string str);
};

#endif // USERINPUTHANDLER_H

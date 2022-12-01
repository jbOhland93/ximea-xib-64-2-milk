#include "../headers/UserInputHandler.h"

#include <ncurses.h>
#include <cstring>

using namespace std;

void UserInputHandler::core()
{   
    const char * prompt = "Enter command: ";
    const char * answer = "You entered: ";

    initscr();
    addstr("\n\n === Welcome to our WIP CLI. ===\n");
    addstr("\nEnter any string and hit enter to display the response.\n");
    addstr("Enter \"quit\" to exit the CLI.\n");
    mvprintw(LINES-2,0,prompt);

    while (true) {
        wmove(stdscr, LINES-2, strlen(prompt));
        clrtoeol();

        char str[1024];
        getstr(str);
        if (0 == strcmp(str, "quit"))
            break;
        else
        {
            mvprintw(LINES-1, 0, answer);
            clrtoeol();
            addstr(str);
        }
    }
    endwin();
}

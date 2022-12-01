#include "../headers/UserInputHandler.h"

#include <ncurses.h>
#include <cstring>

using namespace std;

void UserInputHandler::core()
{
    bool closing = false;

    // Print a welcome message.
    printf("\n\n === Welcome to the deformable-mirror-server application, wrapping the control libraries of ISP System. ===\n");

    // Parse keyboard input (nonblocking).
    //initscr();
    //cbreak();
    //noecho();
    //scrollok(stdscr, TRUE);
    //nodelay(stdscr, TRUE);

char mesg[]="Enter a string: ";         /* message to be appeared on the screen */
 char str[80];
 int row,col;                           /* to store the number of rows and *
                                         * the number of colums of the screen */
 initscr();                             /* start the curses mode */
 getmaxyx(stdscr,row,col);              /* get the number of rows and columns */
 mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);
                                /* print the message at the center of the screen */
 getstr(str);
 mvprintw(LINES - 2, 0, "You Entered: %s", str);
 getch();
 endwin();

    /*while (true) {
        if (getch() == 'q') {
            printw("You pressed q\n");
            break;
        }
        //printw("Running\n");
    }
    endwin();*/
}

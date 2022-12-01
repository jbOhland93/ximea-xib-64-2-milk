#include <iostream>
#include <unistd.h>

#include "headers/UserInputHandler.h"
#include "headers/Acquisitor.h"

using namespace std;

int main()
{
    UserInputHandler ui;
    std::thread uiThread = thread(&UserInputHandler::core, &ui);
    uiThread.join();

    Acquisitor acq;
    cout << "Is acquiring: " << acq.isAcquiring() << "\n";
    cout << "Starting acq result: " << acq.startAcquisition() << "\n";
    cout << "Is acquiring: " << acq.isAcquiring() << "\n";
    usleep(1000000);
    cout << "Stop\n";
    cout << "Is acquiring: " << acq.isAcquiring() << "\n";
    acq.stopAcquisition();
    cout << "Is acquiring: " << acq.isAcquiring() << "\n";
        return 0;
}

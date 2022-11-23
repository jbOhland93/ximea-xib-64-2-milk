#include <iostream>


#include "include/Semaphore.h"
#include "include/Acquisitor.h"

#include <unistd.h>

using namespace std;

int main()
{
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

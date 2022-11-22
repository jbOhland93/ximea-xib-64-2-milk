#include <iostream>


#include "include/Semaphore.h"
#include "include/Acquisitor.h"

#include <unistd.h>

using namespace std;

int main()
{
    Acquisitor acq;
    acq.startAcquisition();
    usleep(1000000);
    acq.stopAcquisition();

	return 0;
}

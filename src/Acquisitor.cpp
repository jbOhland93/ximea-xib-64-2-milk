#include "Acquisitor.h"

#include <unistd.h>
#include <iostream>

using namespace std;

void Acquisitor::startAcquisition()
{
    m_acqThread = thread(&Acquisitor::core, this);
}

bool Acquisitor::isAcquiring()
{
    return mp_sem->getCount() == 1;
}

void Acquisitor::stopAcquisition()
{
    mp_sem->acquire();  // Send a signal to the acquisition thread
    m_acqThread.join(); // Wait for the thred to end
    mp_sem->release();  // Release the semaphore so the acquisition can start again!

}

void Acquisitor::core()
{
    while (isAcquiring())
    {
        // Acquire
        cout << "Acquiring ...\n";
        // Sleep for 100 ms
        usleep(100000);
    }
}

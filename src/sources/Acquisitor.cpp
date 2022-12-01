#include "../headers/Acquisitor.h"

#include <unistd.h>
#include <iostream>

using namespace std;

bool Acquisitor::startAcquisition()
{
    if (!isAcquiring())
    {
        mp_sem->acquire();
        m_acqThread = thread(&Acquisitor::core, this);
        return true;
    }
    else
        return false;
}

bool Acquisitor::isAcquiring()
{
    return mp_sem->getCount() == 0;
}

void Acquisitor::stopAcquisition()
{
    if (isAcquiring())
    {
        mp_sem->release();      // Send a signal to the acquisition thread
        m_acqThread.join();     // Wait for the thred to end
        mp_sem->release();      // Release the semaphore so the acquisition can start again!
        m_acqThread = thread(); // Replace the thread object by a dummy.
    }
}

void Acquisitor::core()
{
    int n = 0;
    while (isAcquiring())
    {
        // Currently, this loop only contains dummy content.
        // Acquire
        cout << "Acquiring " << n << " ...\n";
        n++;
        // Sleep for 100 ms
        usleep(100000);
    }
}

#include "../headers/Acquisitor.h"

#include <unistd.h>
#include <iostream>

#include <time.h>

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
        m_acqThread = thread(); // Replace the thread object by a dummy.
    }
}

unsigned long long int Acquisitor::getFrameCount()
{
    return m_framesAcquired;
}

float Acquisitor::getFPS()
{
    return m_FPS;
}

void Acquisitor::core()
{
    struct timespec last, now;
    clock_gettime(CLOCK_MONOTONIC, &last);
    unsigned long long int lastFramecount = m_framesAcquired;

    while (isAcquiring())
    {
        // Currently, this loop only contains dummy content.
        // Acquire
        m_framesAcquired++;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9 > 1e9)
        {
            m_FPS = (m_framesAcquired-lastFramecount)/((now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9)/1.e9);
            lastFramecount = m_framesAcquired;
            last = now;
        }
        // Sleep for 100 ms
        usleep(100000);
    }
    m_FPS = 0;
}

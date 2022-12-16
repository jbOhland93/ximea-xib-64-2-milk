#include "../headers/Acquisitor.h"

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <time.h>
#include "Acquisitor.h"

using namespace std;

Acquisitor::Acquisitor(HANDLE &cameraHandle)
    : m_cameraHandle(cameraHandle)
{
}

bool Acquisitor::startAcquisition()
{
    if (!isAcquiring())
    {
        mp_acqSem->acquire();
        mp_acqStartSem->acquire();
        m_acqThread = thread(&Acquisitor::core, this);
        mp_acqStartSem->acquire(); // Wait until the thread has decreased this semaphore!
        mp_acqStartSem->release();
        return true;
    }
    else
        return false;
}

bool Acquisitor::isAcquiring()
{
    return mp_acqSem->getCount() == 0;
}

void Acquisitor::stopAcquisition()
{
    if (isAcquiring())
    {
        mp_acqSem->release();      // Send a signal to the acquisition thread
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
    // image buffer
    XI_IMG image;
    memset(&image,0,sizeof(image));
    image.size = sizeof(XI_IMG);

    
    XI_RETURN stat = xiStartAcquisition(m_cameraHandle);
    mp_acqStartSem->release();
    if (stat == XI_OK)
    {
        while (isAcquiring())
        {
            // getting image from camera
            stat = xiGetImage(m_cameraHandle, 5000, &image);
            if (stat == XI_OK)
            {
                // Successfully acquired frame!
                m_framesAcquired++; // Increase frame count
                clock_gettime(CLOCK_MONOTONIC_RAW, &now); // Get timestamp to calculate framerate
                if (now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9 > 1e9)
                {   // One second elapsed since last framerate update. Do update now.
                    m_FPS = (m_framesAcquired-lastFramecount)/((now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9)/1.e9);
                    lastFramecount = m_framesAcquired;
                    last = now;
                }
                unsigned char pixel = *(unsigned char*)image.bp;
                //printf("Image %lld (%dx%d) received from camera. First pixel value: %d\n", m_framesAcquired, (int)image.width, (int)image.height, pixel);
            }
            else
                stopAcquisition();
        }
    }
    m_FPS = 0;

    xiStopAcquisition(m_cameraHandle);
}

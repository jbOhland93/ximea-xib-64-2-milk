#include "Acquisitor.h"

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <time.h>
#include <chrono>

#include "headers/global.h"

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
    clock_gettime(CLOCK_MONOTONIC_RAW, &last);
    unsigned long long int lastFramecount = m_framesAcquired;
    double timediff_ns = -1.;
    // image buffer
    XI_IMG image;
    memset(&image,0,sizeof(image));
    image.size = sizeof(XI_IMG);

    
    XI_RETURN stat = XI_OK;
    if (projectflags::USE_CAM)
        stat = xiStartAcquisition(m_cameraHandle);
    mp_acqStartSem->release();
    if (stat == XI_OK)
    {
        while (isAcquiring())
        {
            // getting image from camera
            if (projectflags::USE_CAM)
                stat = xiGetImage(m_cameraHandle, 5000, &image);
            else
                // Sleep for a ms to limit the frame rate if not camera is used
                this_thread::sleep_for(chrono::milliseconds(1));
            if (stat == XI_OK)
            {
                // Successfully acquired frame!
                m_framesAcquired++; // Increase frame count
                clock_gettime(CLOCK_MONOTONIC_RAW, &now); // Get timestamp to calculate framerate
                timediff_ns = now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9;
                if (timediff_ns > 1e9)
                {   // One second elapsed since last framerate update. Do update now.
                    m_FPS = (m_framesAcquired-lastFramecount)/((now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9)/1.e9);
                    lastFramecount = m_framesAcquired;
                    last = now;
                }
                if (projectflags::USE_CAM)
                    unsigned char pixel = *(unsigned char*)image.bp;
                //printf("Image %lld (%dx%d) received from camera. First pixel value: %d\n", m_framesAcquired, (int)image.width, (int)image.height, pixel);
            }
            else
                stopAcquisition();
        }
    }
    m_FPS = 0;

    if (projectflags::USE_CAM)
        xiStopAcquisition(m_cameraHandle);
}

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
        if (projectflags::USE_CAM)
            m_acqThread = thread(&Acquisitor::coreXI, this);
        else
            m_acqThread = thread(&Acquisitor::coreDummy, this);
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

void Acquisitor::coreXI()
{
    unsigned long long int lastFramecount = m_framesAcquired;
    DWORD lastSecond = 0;
    DWORD lastUSecond = 0;
    // image buffer
    XI_IMG image;
    memset(&image,0,sizeof(image));
    image.size = sizeof(XI_IMG);

    
    XI_RETURN stat = XI_OK;
    stat = xiStartAcquisition(m_cameraHandle);
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
                    // ToDo: Push frame to image processor
                // Do logging, determine FPS etc ...
                m_framesAcquired++; // Increase frame count
                if (lastSecond != image.tsSec)
                {
                    m_FPS = (m_framesAcquired - lastFramecount)*1.e6/(image.tsUSec+1.e6 - lastUSecond);
                    lastSecond = image.tsSec;
                    lastUSecond = image.tsUSec;
                    lastFramecount = m_framesAcquired;
                }
                //unsigned char pixel = *(unsigned char*)image.bp;
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

void Acquisitor::coreDummy()
{
    struct timespec last, now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &last);
    unsigned long long int lastFramecount = m_framesAcquired;
    double timediff_ns = -1.;

    mp_acqStartSem->release();
    while (isAcquiring())
    {
        // getting image from camera (i.e. wait a moment in this dummy case)
        this_thread::sleep_for(chrono::milliseconds(1));
        // Do logging, determine FPS etc ...
        m_framesAcquired++; // Increase frame count
        clock_gettime(CLOCK_MONOTONIC_RAW, &now); // Get timestamp to calculate framerate
        timediff_ns = now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9;
        if (timediff_ns > 1e9)
        {   // One second elapsed since last framerate update. Do update now.
            m_FPS = (m_framesAcquired-lastFramecount)/((now.tv_nsec - last.tv_nsec + (now.tv_sec-last.tv_sec)*1e9)/1.e9);
            lastFramecount = m_framesAcquired;
            last = now;
        }
    }
    
    m_FPS = 0;

    if (projectflags::USE_CAM)
        xiStopAcquisition(m_cameraHandle);
}

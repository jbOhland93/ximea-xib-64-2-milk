#ifndef ACQUISITOR_H
#define ACQUISITOR_H

#include <thread>
#include <memory>
#include <m3api/xiApi.h>
#include "../headers/Semaphore.h"
#include "ImageProcessor.h"

// A class that handles the acquisition of the camera
class Acquisitor
{
    public:
        // Ctor, taking the camera handle
        Acquisitor(HANDLE& cameraHandle);

        // Starts the acquisition loop.
        // Returns true if the loop has been started.
        // Returns false if the loop is already running.
        bool startAcquisition();
        // Returns true of the acquisition loop is running
        bool isAcquiring();
        // Stops the acquisition loop
        void stopAcquisition();
        // Returns the current frame count
        unsigned long long int getFrameCount();
        // Returns the current estimated framerage in frames per second
        float getFPS();

    private:
        const HANDLE& m_cameraHandle;
        // The semaphore to stop the image acquisition once running
        const std::shared_ptr<Semaphore> mp_acqSem{ new Semaphore(1) };
        // The semaphore to signal that the acquisition started successfully
        const std::shared_ptr<Semaphore> mp_acqStartSem{ new Semaphore(1) };
        // The acquisition thread
        std::thread m_acqThread;
        // The total number of frames which has been acquired
        unsigned long long int m_framesAcquired = 0;
        // The current frames per second
        float m_FPS = 0.;
        const ImageProcessor m_imProc;

        // Standard ctor, not to be used
        Acquisitor();

        // The acquisition loop, handling the raw acquisition of the camera.
        // This will be launched as separate thread.
        // There are two versions: One handling the actual camera and one dummy.
        void coreXI();
        void coreDummy();
};

#endif // ACQUISITOR_H

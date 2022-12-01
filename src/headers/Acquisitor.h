#ifndef ACQUISITOR_H
#define ACQUISITOR_H

#include <thread>
#include <memory>
#include "../headers/Semaphore.h"

// A class that handles the acquisition of the camera
class Acquisitor
{
    public:
        // Starts the acquisition loop.
        // Returns true if the loop has been started.
        // Returns false if the loop is already running.
        bool startAcquisition();
        // Returns true of the acquisition loop is running
        bool isAcquiring();
        // Stops the acquisition loop
        void stopAcquisition();

    private:
        // The semaphore to stop the image acquisition once running
        const std::shared_ptr<Semaphore> mp_sem{ new Semaphore(1) };
        // The acquisition thread
        std::thread m_acqThread;

        // The acquisition loop, handling the raw acquisition of the camera.
        // This will be launched as separate thread.
        void core();
};

#endif // ACQUISITOR_H
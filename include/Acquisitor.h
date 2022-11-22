#ifndef ACQUISITOR_H
#define ACQUISITOR_H

#include <thread>
#include <memory>
#include "../include/Semaphore.h"

class Acquisitor
{
    public:
        void startAcquisition();
        bool isAcquiring();
        void stopAcquisition();

    private:
        const std::shared_ptr<Semaphore> mp_sem{ new Semaphore(1) };
        std::thread m_acqThread;

        void core();
};

#endif // ACQUISITOR_H

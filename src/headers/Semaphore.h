#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

// A class for synchronizing threads
class Semaphore
{
    public:
        // Constructor
        // count is the number of max. available acquisition calls.
        Semaphore(int count = 1);
        // Releases semaphore, increases count by one.
        void release();
        // Acquires semaphore, decreases count by one.
        void acquire();
        // Returns the number of available acquisition calls.
        int getCount();

    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
};

#endif // SEMAPHORE_H

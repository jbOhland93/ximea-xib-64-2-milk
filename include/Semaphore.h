#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore
{
    public:
        Semaphore(int count = 1);
        void release();
        void acquire();
        int getCount();

    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
};

#endif // SEMAPHORE_H

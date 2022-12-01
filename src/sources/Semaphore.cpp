#include "../headers/Semaphore.h"
#include <stdio.h>

Semaphore::Semaphore(int count)
    : count(count)
{
}

void Semaphore::release()
{
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    //notify the waiting thread
    cv.notify_one();
}

void Semaphore::acquire()
{
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0) {
        // wait on the mutex until notify is called
        cv.wait(lock);
    }
    count--;
}

int Semaphore::getCount()
{
    return count;
}

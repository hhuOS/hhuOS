#include "ThreadPool.h"

ThreadPool::ThreadPool(uint32_t size) : threads(size) {
    for (auto &thread : threads) {
        thread = ThreadPoolWorker(this);
    }
}

ThreadPool::~ThreadPool() {
    stopWorking(true);
}

void ThreadPool::addWork(void (*func)()) {
    workQueue.push(func);
}

void ThreadPool::startWorking() {
    if(working) {
        return;
    }

    working = true;

    for (auto &thread : threads) {
        thread.start();
    }
}

void ThreadPool::stopWorking(bool force) {
    if(!working) {
        return;
    }

    working = false;

    Scheduler *scheduler = Scheduler::getInstance();

    if(!force) {
        bool allThreadsFinished = false;

        // Wait until work queue is empty
        while(!workQueue.isEmpty());

        // Wait for every thread to finish working
        while(!allThreadsFinished) {
            allThreadsFinished = true;

            for (auto &thread : threads) {
                if(thread.isWorking) {
                    allThreadsFinished = false;
                }
            }
        }
    }

    for (auto &thread : threads) {
        scheduler->kill(thread);
    }
}

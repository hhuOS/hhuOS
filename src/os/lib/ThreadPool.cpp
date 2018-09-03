#include "ThreadPool.h"

ThreadPool::ThreadPool(uint32_t size) : threads(size) {
    for (auto &thread : threads) {
        thread = ThreadPoolWorker(this);
    }
}

void ThreadPool::addWork(void (*func)()) {
    workQueue.push(func);
}

void ThreadPool::startWorking() {
    working = true;

    for (auto &thread : threads) {
        thread.start();
    }
}

bool ThreadPool::isWorking() {
    return working;
}

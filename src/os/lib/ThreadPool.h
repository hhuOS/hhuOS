#ifndef HHUOS_THREADPOOL_H
#define HHUOS_THREADPOOL_H

#include <lib/util/Queue.h>
#include <kernel/threads/WorkerThread.h>
#include <lib/util/ThreadSafeBlockingQueue.h>
#include <lib/lock/Mutex.h>
#include <kernel/threads/Scheduler.h>

class ThreadPool {

private:

    struct ThreadPoolWorker : public Thread {

        bool isWorking = false;

        ThreadPool *pool = nullptr;

        ThreadPoolWorker() = default;

        explicit ThreadPoolWorker(ThreadPool *pool) : pool(pool) {

        }

        ThreadPoolWorker(const ThreadPoolWorker &other) {
            this->pool = other.pool;
        }

        ThreadPoolWorker& operator=(const ThreadPoolWorker &other) {
            this->pool = other.pool;

            return *this;
        }

        void run() override {
            Scheduler *scheduler = Scheduler::getInstance();

            while(true) {
                while(pool->workQueue.isEmpty()) {
                    scheduler->yield();
                }

                void (*work)() = pool->workQueue.pop();

                isWorking = true;
                work();
                isWorking = false;
            }
        }

    };

private:

    Util::ThreadSafeBlockingQueue<void (*)()> workQueue;
    Util::Array<ThreadPoolWorker> threads;

    bool working = false;

public:

    explicit ThreadPool(uint32_t size = 16);

    ~ThreadPool();

    void addWork(void (*func)());

    void startWorking();

    void stopWorking(bool force = false);

};

#endif

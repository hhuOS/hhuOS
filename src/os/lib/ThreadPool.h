#ifndef HHUOS_THREADPOOL_H
#define HHUOS_THREADPOOL_H

#include <lib/util/Queue.h>
#include <kernel/threads/WorkerThread.h>
#include <lib/util/BlockingQueue.h>
#include <lib/lock/Mutex.h>

class ThreadPool {

private:

    struct ThreadPoolWorker : public Thread {

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
            while(true) {
                pool->mutex.acquire();

                void (*work)() = pool->workQueue.pop();

                pool->mutex.release();

                work();
            }
        }

    };

private:

    Mutex mutex;
    Util::BlockingQueue<void (*)()> workQueue;
    Util::Array<ThreadPoolWorker> threads;

    bool working = false;

public:

    explicit ThreadPool(uint32_t size = 16);

    ~ThreadPool() = default;

    void addWork(void (*func)());

    void startWorking();

    bool isWorking();

};

#endif

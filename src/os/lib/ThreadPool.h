/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

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

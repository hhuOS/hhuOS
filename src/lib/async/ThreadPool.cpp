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

#include "kernel/thread/Scheduler.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(uint32_t size) : threads(size) {
    for (auto &thread : threads) {
        thread = ThreadPoolWorker(this, &workQueueLock);
    }
}

ThreadPool::~ThreadPool() {
    stopWorking(true);
}

void ThreadPool::addWork(void (*func)()) {
    workQueueLock.acquire();

    workQueue.push(func);

    workQueueLock.release();
}

void ThreadPool::startWorking() {
    if(hasStarted) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                "Trying to restart an already running instance of ThreadPool!");
    }

    hasStarted = true;

    for (auto &thread : threads) {
        thread.start();
    }
}

void ThreadPool::stopWorking(bool force) {
    if(!hasStarted) {
        return;
    }

    Kernel::Scheduler &scheduler = Kernel::Scheduler::getInstance();

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

    for(auto &thread : threads) {
        scheduler.kill(thread);
    }
}

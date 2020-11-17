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

#ifndef __Thread_include__
#define __Thread_include__

#include "kernel/thread/ThreadState.h"

#include <cstdint>
#include <lib/system/IdGenerator.h>
#include "lib/string/String.h"

namespace Kernel {

class Thread {

    friend class Scheduler;

public:

    class Stack {

    public:

        explicit Stack(uint32_t size);

        ~Stack();

        uint8_t *getStart();

    private:

        uint8_t *stack;

        uint32_t size;

    };

protected:

    Thread();

    explicit Thread(const String &name);

    Thread(const String &name, uint8_t priority);

public:

    Thread(const Thread &copy) = delete;

    Thread &operator=(const Thread &other) = delete;

    virtual ~Thread() = default;

public:

    InterruptFrame *interruptFrame = nullptr;

    Context *kernelContext = nullptr;

    /**
     * Returns this Thread's thread id.
     *
     * @return This Thread's thread id
     */
    uint32_t getId() const;

    /**
     * Returns this Thread's name.
     *
     * @return This Thread's name
     */
    String getName() const;

    uint8_t getPriority() const;

    bool hasStarted() const;

    bool hasFinished() const;

    void setPriority(uint8_t priority);

    // uint32_t getPid();



    /**
     * Starts this Thread.
     */
    virtual void start() final;

    void join() const;

    virtual void run() = 0;

    virtual Stack& getUserStack() = 0;

    virtual Stack& getKernelStack() = 0;

protected:

    static void yield();

    InterruptFrame& getInterruptFrame() const;

    Context& getKernelContext() const;

    static const uint32_t STACK_SIZE_DEFAULT = 4096;

private:

    uint32_t id;

    uint8_t priority;

    String name;

    bool started = false;

    bool finished = false;

    static IdGenerator idGenerator;

};

}

#endif

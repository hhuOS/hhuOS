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

#include "kernel/threads/ThreadState.h"

#include <cstdint>
#include <lib/String.h>

class Thread {

public:

    InterruptFrame *interruptFrame;

    Context *context;

    Thread();

    explicit Thread(const String &name);

    Thread(const String &name, uint8_t priority);

    Thread(const Thread &copy) = delete;

    Thread &operator=(const Thread &other) = delete;

    virtual ~Thread() = default;

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

    void setPriority(uint8_t priority);

    /**
     * Starts this Thread.
     */
    virtual void start() final;

    virtual void run () = 0;

protected:

    void yield();

private:

    uint32_t id;

    uint8_t priority;

    String name;

    class Stack {

    public:

        explicit Stack(uint32_t size);

        ~Stack();

        uint8_t *getStart();

    private:

        uint8_t *stack;

        uint32_t size;

    };

    Stack stack;

    /**
     * Initializes this Thread.
     */
    void init();

    /* 4KB Stack Size */
    static const uint32_t   STACK_SIZE_DEFAULT            = 4096;

};

#endif

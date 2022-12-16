/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_THREAD_H
#define HHUOS_THREAD_H

#include <cstdint>

#include "lib/util/memory/String.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Util {
namespace Async {
class Runnable;
template <typename T> class IdGenerator;
}  // namespace Async
}  // namespace Util

namespace Kernel {

class Process;
struct Context;
struct InterruptFrame;

class Thread {

    friend class ThreadScheduler;
    friend class Scheduler;

public:

    class Stack {

    public:

        Stack(const Stack &other) = delete;

        Stack &operator=(const Stack &other) = delete;

        ~Stack();

        static Stack* createKernelStack(uint32_t size);

        static Stack* createUserStack(uint32_t size);

        static Stack* createMainUserStack();

        [[nodiscard]] uint8_t* getStart() const;

    private:

        explicit Stack(uint8_t *stack, uint32_t size);

        uint8_t *stack;
        uint32_t size;

    };

    /**
     * Copy Constructor.
     */
    Thread(const Thread &other) = delete;

    /**
     * Assignment operator.
     */
    Thread &operator=(const Thread &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Thread();

    static Thread& createKernelThread(const Util::Memory::String &name, Process &parent, Util::Async::Runnable *runnable);

    static Thread &createUserThread(const Util::Memory::String &name, Process &parent, uint32_t eip,
                                    Util::Async::Runnable *runnable);

    static Thread& createMainUserThread(const Util::Memory::String &name, Process &parent, uint32_t eip, uint32_t argc, char **argv, void *envp, uint32_t heapStartAddress);

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] Util::Memory::String getName() const;

    [[nodiscard]] Context* getContext() const;

    [[nodiscard]] Process& getParent() const;

    [[nodiscard]] uint8_t* getFpuContext() const;

    void join();

    void unblockJoinList();

    virtual void run();

private:

    Thread(const Util::Memory::String &name, Process &parent, Util::Async::Runnable *runnable, Thread::Stack *kernelStack, Thread::Stack *userStack);

    uint32_t id;
    Util::Memory::String name;
    Process &parent;
    Util::Async::Runnable *runnable;

    Stack *kernelStack;
    Stack *userStack;

    InterruptFrame &interruptFrame;
    Context *kernelContext;
    uint8_t *fpuContext;

    Util::Data::ArrayList<Thread*> joinList;
    Util::Async::Spinlock joinLock;

    static Util::Async::IdGenerator<uint32_t> idGenerator;
    static const constexpr uint32_t DEFAULT_STACK_SIZE = 4096;
};

}

#endif

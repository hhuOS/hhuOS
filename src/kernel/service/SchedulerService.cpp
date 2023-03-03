/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdarg.h>

#include "kernel/system/System.h"
#include "SchedulerService.h"
#include "ProcessService.h"
#include "kernel/service/SchedulerService.h"
#include "device/cpu/Fpu.h"
#include "kernel/log/Logger.h"
#include "kernel/process/Process.h"
#include "kernel/process/SchedulerCleaner.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/system/SystemCall.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Kernel {

Logger SchedulerService::log = Logger::get("Scheduler");

SchedulerService::SchedulerService() {
    defaultFpuContext = static_cast<uint8_t*>(System::getService<MemoryService>().allocateKernelMemory(512, 16));
    Util::Address<uint32_t>(defaultFpuContext).setRange(0, 512);

    if (Device::Fpu::isAvailable()) {
        log.info("FPU detected -> Enabling FPU context switching");
        fpu = new Device::Fpu(defaultFpuContext);
        fpu->plugin();
    } else {
        log.warn("No FPU present");
    }

    SystemCall::registerSystemCall(Util::System::YIELD, [](uint32_t, va_list) -> bool {
        System::getService<SchedulerService>().yield();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::GET_CURRENT_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &schedulerService = System::getService<SchedulerService>();
        auto &threadId = *va_arg(arguments, uint32_t*);

        threadId = schedulerService.getCurrentThread().getId();
        return true;
    });

    SystemCall::registerSystemCall(Util::System::CREATE_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 4) {
            return false;
        }

        auto &processService = System::getService<ProcessService>();
        auto &schedulerService = System::getService<SchedulerService>();
        auto *name = va_arg(arguments, const char*);
        auto *runnable = va_arg(arguments, Util::Async::Runnable*);
        auto eip = va_arg(arguments, uint32_t);
        auto &threadId = *va_arg(arguments, uint32_t*);

        auto &thread = Kernel::Thread::createUserThread(name, processService.getCurrentProcess(), eip, runnable);

        threadId = thread.getId();
        schedulerService.ready(thread);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::SLEEP, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &schedulerService = System::getService<SchedulerService>();
        auto &time = *va_arg(arguments, Util::Time::Timestamp*);

        schedulerService.sleep(time);
        return true;
    });

    SystemCall::registerSystemCall(Util::System::JOIN_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 1) {
            return false;
        }

        auto &schedulerService = System::getService<SchedulerService>();
        auto threadId = va_arg(arguments, uint32_t);

        auto *thread = schedulerService.getThread(threadId);
        if (thread != nullptr) {
            thread->join();
        }

        return true;
    });

    SystemCall::registerSystemCall(Util::System::EXIT_THREAD, [](uint32_t paramCount, va_list arguments) -> bool {
        System::getService<SchedulerService>().exitCurrentThread();
        return true;
    });
}

void SchedulerService::kickoffThread() {
    scheduler.getCurrentThread().run();
    scheduler.exit();
}

void SchedulerService::startScheduler() {
    auto &processService = System::getService<ProcessService>();
    cleaner = new Kernel::SchedulerCleaner();
    auto &schedulerCleanerThread = Kernel::Thread::createKernelThread("Scheduler-Cleaner", processService.getKernelProcess(), cleaner);
    ready(schedulerCleanerThread);
    
    scheduler.start();
}

void SchedulerService::ready(Thread &thread) {
    scheduler.ready(thread);
}

void SchedulerService::lockScheduler() {
    scheduler.lock.acquire();
}

void SchedulerService::unlockScheduler() {
    scheduler.lock.release();
}

void SchedulerService::yield() {
    scheduler.yield();
}

Thread& SchedulerService::getCurrentThread() {
    return scheduler.getCurrentThread();
}

Thread *SchedulerService::getThread(uint32_t id) {
    return scheduler.getThread(id);
}

void SchedulerService::cleanup(Thread *thread) {
    fpu->checkTerminatedThread(*thread);
    cleaner->cleanup(thread);
}

void SchedulerService::cleanup(Process *process) {
    cleaner->cleanup(process);
}

void SchedulerService::block() {
    scheduler.block();
}

void SchedulerService::unblock(Thread &thread) {
    auto &processService = System::getService<ProcessService>();
    auto &process = thread.getParent();
    if (processService.isProcessActive(process.getId())) {
        scheduler.unblock(thread);
    } else {
        cleanup(&thread);
    }
}

void SchedulerService::kill(Thread &thread) {
    scheduler.kill(thread);
}

void SchedulerService::killWithoutLock(Thread &thread) {
    scheduler.killWithoutLock(thread);
}

void SchedulerService::exitCurrentThread() {
    scheduler.exit();
}

uint8_t *SchedulerService::getDefaultFpuContext() {
    return defaultFpuContext;
}

void SchedulerService::sleep(const Util::Time::Timestamp &time) {
    scheduler.sleep(time);
}

}
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

#ifndef HHUOS_MEMORYMANAGERTESTAPP_H
#define HHUOS_MEMORYMANAGERTESTAPP_H

#include <kernel/memory/manager/MemoryManager.h>
#include "kernel/threads/Thread.h"
#include "lib/Random.h"
#include "lib/libc/printf.h"
#include "apps/Application.h"
#include "kernel/Kernel.h"

class MemoryManagerTest {

private:

    Random random;

    MemoryManager &manager;

    String managerName;

    bool deleteManager = false;

    void **objects = nullptr;

    void *memory = nullptr;

    uint32_t numAllocs;

    uint32_t maxAllocSize;

private:

    bool testAlloc();

    void shuffle();

public:

    MemoryManagerTest(const String &managerType, uint32_t memorySize, uint32_t numAllocs,
                      uint32_t maxAllocSize = 8192, const String &name = "");

    MemoryManagerTest(MemoryManager &manager, uint32_t memorySize, uint32_t numAllocs,
                      uint32_t maxAllocSize = 8192, const String &name = "");

    MemoryManagerTest(const MemoryManagerTest &copy) = delete;

    ~MemoryManagerTest();

    void run();
};

#endif
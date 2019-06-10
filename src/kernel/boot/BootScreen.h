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

#ifndef __BootScreen_include__
#define __BootScreen_include__

#include "lib/string/String.h"
#include "lib/file/File.h"
#include "lib/graphic/Image.h"
#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "kernel/log/Logger.h"
#include "kernel/memory/manager/MemoryManager.h"
#include "BootCoordinator.h"

class BootScreen : public Thread {

public:

    explicit BootScreen(BootCoordinator &coordinator);

    ~BootScreen() override = default;

    BootScreen(const BootScreen &other) = delete;

    BootScreen &operator=(const BootScreen &other) = delete;

    void drawScreen();

    void drawComponentStatus(uint32_t index, uint16_t posY);

    void drawHeapStatus(uint16_t basePosY);

    void init(uint16_t xres, uint16_t yres, uint8_t bpp);

    void finish();

    void run() override;

private:

    BootCoordinator &coordinator;

    volatile bool isRunning = false;

    LinearFrameBuffer *lfb = nullptr;

    Font *font = &std_font_8x16;

    Image *logo = nullptr;

    Util::Array<BootComponent*> components;
    Util::Array<String> componentNames;

    char *heapStatusBuffers[3];
    char *activeThreadsBuffer;

    MemoryManager *kernelHeapManager = nullptr;
    MemoryManager *pageFrameAllocator = nullptr;
    MemoryManager *ioMemoryManager = nullptr;

    uint32_t kernelMemory = 0;
    uint32_t physicalMemory = 0;
    uint32_t ioMemory = 0;

    static const constexpr uint32_t BUFFER_SIZE = 64;
};


#endif

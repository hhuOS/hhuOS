/*
 * Copyright (C) 2018  Filip Krakowski, Fabian Ruhland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AsciimationApp_include__
#define __AsciimationApp_include__

#include "kernel/threads/Thread.h"
#include "lib/file/File.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "kernel/services/TimeService.h"

/**
 * @author Filip Krakowski
 */
class AsciimationApp : public Thread, public Receiver {

private:

    uint32_t readDelay();

    void readLine(char *buf);

    void printFrame();

    char fileName[4096];

    uint64_t fileLength = 0;

    File *file = nullptr;

    char* buffer = nullptr;

    GraphicsService *graphicsService = nullptr;

    FileSystem *fileSystem = nullptr;

    TimeService *timeService = nullptr;

    EventBus *eventBus = nullptr;

    uint16_t posX, posY;

public:

    AsciimationApp();

    AsciimationApp(const AsciimationApp &copy) = delete;

    ~AsciimationApp() override = default;

    void onEvent(const Event &event) override;

    void run() override;
};

#endif

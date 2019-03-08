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

#ifndef __KernelEntry_include__
#define __KernelEntry_include__


#include <cstdint>
#include <kernel/Bootscreen.h>
#include <lib/file/tar/Archive.h>
#include <kernel/services/ModuleLoader.h>
#include <kernel/threads/IdleThread.h>

#include <lib/util/SmartPointer.h>
#include <kernel/events/input/KeyEvent.h>

class GatesOfHell {

public:

    GatesOfHell() = delete;

    virtual ~GatesOfHell() = delete;

    GatesOfHell(const GatesOfHell &other) = delete;

    GatesOfHell &operator=(const GatesOfHell &other) = delete;

    static int32_t enter();

private:

    static void registerServices();

    static void afterInitrdModHook();

    static void afterFsInitModHook();

    static void initializeGraphics();

    static void afterPciScanModHook();

    static void initializeMemoryManagers();

    static void initializePorts();

    static bool loadModule(const String &path);

    static Logger &log;

    static ModuleLoader *moduleLoader;

    static GraphicsService *graphicsService;

    static EventBus *eventBus;

    static Bootscreen *bootscreen;

    static IdleThread *idleThread;

    static uint16_t xres;

    static uint16_t yres;

    static uint8_t bpp;
};


#endif

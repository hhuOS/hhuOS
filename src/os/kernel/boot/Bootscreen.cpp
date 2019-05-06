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

#include <lib/graphic/Colors.h>
#include <devices/graphics/text/fonts/Fonts.h>
#include <BuildConfig.h>
#include <lib/file/bmp/Bmp.h>
#include <kernel/services/GraphicsService.h>
#include <kernel/services/TimeService.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/threads/Scheduler.h>
#include "Bootscreen.h"
#include "kernel/Kernel.h"

auto versionString = String::format("hhuOS %s - git %s (%s)", BuildConfig::VERSION, BuildConfig::GIT_REV, BuildConfig::GIT_BRANCH);
auto buildDate = String::format("Build date: %s", BuildConfig::BUILD_DATE);

Bootscreen::Bootscreen(BootCoordinator &coordinator) : coordinator(coordinator) {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    kernelHeapManager = SystemManagement::getKernelHeapManager();
    pageFrameAllocator = SystemManagement::getInstance().getPageFrameAllocator();
    ioMemoryManager = SystemManagement::getInstance().getIOMemoryManager();

    kernelMemory = kernelHeapManager->getEndAddress() - kernelHeapManager->getStartAddress();
    physicalMemory = pageFrameAllocator->getEndAddress() - pageFrameAllocator->getStartAddress();
    ioMemory = ioMemoryManager->getEndAddress() - ioMemoryManager->getStartAddress();
}

void Bootscreen::drawScreen() {

    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_DARK_BLUE);

    lfb->placeString(*font, 50, 10, static_cast<const char*>(versionString), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->placeString(*font, 50, 15, static_cast<const char*>(buildDate), Colors::HHU_GRAY, Colors::INVISIBLE);

    if(logo != nullptr) {
        logo->draw(static_cast<uint16_t>((lfb->getResX() - logo->getWidth()) / 2),
                   static_cast<uint16_t>((lfb->getResY() - logo->getHeight()) / 2));
    }

    Util::Array<BootComponent*> components = coordinator.getComponents();

    uint32_t componentBasePosY = lfb->getResY() - components.length() * font->get_char_height();

    for(uint32_t i = 0; i < components.length(); i++) {
        drawComponentStatus(*components[i], static_cast<uint16_t>(componentBasePosY + i * font->get_char_height()));
    }

    uint32_t heapBasePosY = lfb->getResY() - 7u * font->get_char_height();

    drawHeapStatus(static_cast<uint16_t>(heapBasePosY));

    lfb->show();
}

void Bootscreen::drawComponentStatus(BootComponent &component, uint16_t posY) {

    String state = "";
    Color stateColor;

    if(component.isWaiting()) {
        state = "Waiting";
        stateColor = Colors::HHU_YELLOW;
    } else if(component.hasFinished()) {
        state = "Finished";
        stateColor = Colors::HHU_GREEN;
    } else {
        state = "Running";
        stateColor = Colors::HHU_TURQUOISE;
    }

    String name = component.getName();

    lfb->drawString(*font, 0, posY, static_cast<const char*>(name), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->drawString(*font, static_cast<uint16_t>(name.length() * font->get_char_width()), posY, "[",
            Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->drawString(*font, static_cast<uint16_t>((name.length() + 1) * font->get_char_width()), posY,
            static_cast<const char*>(state), stateColor, Colors::INVISIBLE);
    lfb->drawString(*font, static_cast<uint16_t>((name.length() + state.length() + 1) * font->get_char_width()), posY,
            "]", Colors::HHU_GRAY, Colors::INVISIBLE);
}

void Bootscreen::drawHeapStatus(uint16_t basePosY) {

    auto posX = static_cast<uint16_t>(lfb->getResX() - 35 * font->get_char_width());

    uint32_t usedKernelMemory = kernelMemory - kernelHeapManager->getFreeMemory();
    uint32_t usedPhysicalMemory = physicalMemory - pageFrameAllocator->getFreeMemory();
    uint32_t usedIoMemory = ioMemory - ioMemoryManager->getFreeMemory();

    String kernel = String::format("Kernel: %u/%u KiB", usedKernelMemory / 1024, kernelMemory / 1024);
    String physical = String::format("Physical: %u/%u KiB", usedPhysicalMemory / 1024, physicalMemory / 1024);
    String io = String::format("IO: %u/%u KiB", usedIoMemory / 1024, ioMemory / 1024);

    String threads = String::format("Active Threads: %u", Scheduler::getInstance().getThreadCount());

    lfb->drawString(*font, posX, basePosY, "Heap Status:", Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->drawString(*font, posX, static_cast<uint16_t>(basePosY + font->get_char_height() * 1),
            static_cast<const char*>(kernel), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->drawString(*font, posX, static_cast<uint16_t>(basePosY + font->get_char_height() * 2),
            static_cast<const char*>(physical), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->drawString(*font, posX, static_cast<uint16_t>(basePosY + font->get_char_height() * 3),
            static_cast<const char*>(io), Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->drawString(*font, posX, static_cast<uint16_t>(basePosY + font->get_char_height() * 5),
                    static_cast<const char*>(threads), Colors::HHU_GRAY, Colors::INVISIBLE);
}

void Bootscreen::init(uint16_t xres, uint16_t yres, uint8_t bpp) {

    lfb->init(xres, yres, bpp);

    // Don't use High-Res mode on CGA, as it looks bad.
    if(lfb->getDepth() == 1) {
        lfb->init(320, 200, 2);
    }

    lfb->enableDoubleBuffering();

    logo = Bmp::load("/os/boot-logo.bmp");

    if (logo != nullptr) {
        double scaling = (static_cast<double>(lfb->getResY()) / 600);

        if (scaling >= 1) {
            if(scaling - static_cast<uint8_t>(scaling) >= 0.8) {
                scaling++;
            }

            logo->scaleUp(static_cast<uint8_t>(scaling));
        } else {
            scaling = 1 / scaling;

            if(scaling - static_cast<uint8_t>(scaling) >= 0.2) {
                scaling++;
            }

            logo->scaleDown(static_cast<uint8_t>(scaling));
        }
    }

    isRunning = true;

    start();
}

void Bootscreen::finish() {

    isRunning = false;

    lfb->disableDoubleBuffering();

    lfb->clear();

    delete logo;
}

void Bootscreen::run() {

    auto *timeService = Kernel::getService<TimeService>();

    while (isRunning) {
        drawScreen();

        timeService->msleep(500);
    }
}

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

#include "kernel/core/System.h"
#include "device/misc/Pic.h"
#include "DebugService.h"
#include "kernel/core/Management.h"
#include "lib/libc/printf.h"
#include "SoundService.h"
#include "InputService.h"
#include "GraphicsService.h"

namespace Kernel {

DebugService::DebugService() : pic(Pic::getInstance()) {
    timeService = System::getService<TimeService>();
    keyboard = System::getService<InputService>()->getKeyboard();
    lfb = System::getService<GraphicsService>()->getLinearFrameBuffer();
}

void DebugService::dumpMemory(uint32_t address, size_t lines) {
    volatile uint8_t *tmp = (volatile uint8_t *) address;

    printf("|--------------------------------------------------------------------------------|\n");
    printf("|                                   MEMDUMP                                      |\n");
    printf("|--------------------------------------------------------------------------------|\n");

    for (uint32_t i = 0; i < lines; i++) {
        printf("| %08x   ", tmp);

        for (uint8_t j = 0; j < 16; j++) {
            printf("%02x ", tmp[j]);

            if (j == 7) {
                printf(" ");
            }
        }

        printf("  ");

        for (uint8_t j = 0; j < 16; j++) {
            printf("%c", sanitize(tmp[j]));
        }

        tmp += 16;

        printf(" |\n");

    }

    printf("|--------------------------------------------------------------------------------|\n");
}

char DebugService::sanitize(char c) {
    if (c < 0x40 || c > 0x7E) {
        return '.';
    }

    return c;
}

void DebugService::dumpMemoryList() {

    Management::getKernelHeapManager()->dump();

    while (!keyboard->isKeyPressed(28));

    timeService->msleep(500);

}

void DebugService::printPic() {

    for (uint8_t i = 0; i < 16; i++) {

        bool status = pic.status(Pic::Interrupt(i));

        if (status) {
            lfb->placeFilledRect(10 + 5 * i, 85, 2, 5, Colors::RED);
        } else {
            lfb->placeFilledRect(10 + 5 * i, 85, 2, 5, Colors::GREEN);
        }
    }
}

}
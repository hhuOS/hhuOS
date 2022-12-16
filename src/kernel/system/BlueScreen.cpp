/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/paging/MemoryLayout.h"
#include "device/cpu/Cpu.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/system/System.h"
#include "BlueScreen.h"
#include "device/cpu/IoPort.h"
#include "kernel/process/ThreadState.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/memory/Address.h"

namespace Kernel {

BlueScreen::Mode BlueScreen::mode = LFB;
uint32_t BlueScreen::fbAddress = 0;
uint16_t BlueScreen::fbResX = 0;
uint16_t BlueScreen::fbResY = 0;
uint16_t BlueScreen::fbColorDepth = 0;
uint16_t BlueScreen::fbPitch = 0;
uint32_t BlueScreen::maxStacktraceSize = 0;
uint16_t BlueScreen::posX = OFFSET_X;
uint16_t BlueScreen::posY = OFFSET_Y;

void BlueScreen::setLfbMode(uint32_t address, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch) {
    mode = LFB;
    fbAddress = address;
    fbResX = resolutionX;
    fbResY = resolutionY;
    fbColorDepth = colorDepth;
    fbPitch = pitch;
    maxStacktraceSize = (resolutionY / Util::Graphic::Fonts::TERMINAL_FONT.getCharHeight()) - 12;
}

void BlueScreen::setCgaMode(uint32_t address, uint16_t columns, uint16_t rows) {
    mode = CGA;
    fbAddress = address;
    fbResX = columns;
    fbResY = rows;
    fbColorDepth = 0;
    fbPitch = 0;
    maxStacktraceSize = rows - 12;
}

void BlueScreen::show(const InterruptFrame &frame) {
    auto address = Util::Memory::Address<uint32_t>(fbAddress);
    auto lfb = Util::Graphic::LinearFrameBuffer(&address, fbResX, fbResY, fbColorDepth, fbPitch);
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);

    clear(pixelDrawer);

    print(stringDrawer, "[PANIC][Exception ");
    printDecNumber(stringDrawer, frame.interrupt);
    print(stringDrawer, "] ");
    printLine(stringDrawer, Device::Cpu::getExceptionName(frame.interrupt));

    if (Util::Memory::Address<uint32_t>(Util::System::errorMessage).stringLength() > 0) {
        printLine(stringDrawer, "");
        printLine(stringDrawer, Util::System::errorMessage);
    }

    uint32_t cr0 = 0;
    asm volatile(
            "mov %%cr0, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr0)
            : "eax"
            );

    uint32_t cr4 = 0;
    asm volatile(
            "mov %%cr4, %%eax;"
            "mov %%eax, (%0);"
            : :
            "r"(&cr4)
            : "eax"
            );

    printLine(stringDrawer, "");

    print(stringDrawer, "eax=");
    printHexNumber(stringDrawer, frame.eax);
    posX = OFFSET_X + 15;
    print(stringDrawer, " ebx=");
    printHexNumber(stringDrawer, frame.ebx);
    posX = OFFSET_X + 30;
    print(stringDrawer, " ecx=");
    printHexNumber(stringDrawer, frame.ecx);
    posX = OFFSET_X + 45;
    print(stringDrawer, " edx=");
    printHexNumber(stringDrawer, frame.edx);
    printLine(stringDrawer, "");

    print(stringDrawer, "esp=");
    printHexNumber(stringDrawer, frame.esp);
    posX = OFFSET_X + 15;
    print(stringDrawer, " ebp=");
    printHexNumber(stringDrawer, frame.ebp);
    posX = OFFSET_X + 30;
    print(stringDrawer, " esi=");
    printHexNumber(stringDrawer, frame.esi);
    posX = OFFSET_X + 45;
    print(stringDrawer, " edi=");
    printHexNumber(stringDrawer, frame.edi);
    printLine(stringDrawer, "");

    print(stringDrawer, "eflags=");
    printHexNumber(stringDrawer, frame.eflags);
    posX = OFFSET_X + 15;
    print(stringDrawer, " eip=");
    printHexNumber(stringDrawer, frame.eip);
    posX = OFFSET_X + 30;
    print(stringDrawer, " cr0=");
    printHexNumber(stringDrawer, cr0);
    posX = OFFSET_X + 45;
    print(stringDrawer, " cr4=");
    printHexNumber(stringDrawer, cr4);

    printLine(stringDrawer, "");
    printLine(stringDrawer, "");

    auto *ebp = reinterpret_cast<uint32_t*>(frame.ebp);
    uint32_t eip = frame.eip;
    uint32_t i;

    for (i = 0; i < maxStacktraceSize && eip != 0; i++) {
        print(stringDrawer, "#");
        printDecNumber(stringDrawer, i);
        posX = OFFSET_X + 4;

        printHexNumber(stringDrawer, eip);
        posX = OFFSET_X;
        posY++;

        eip = ebp[1];
        ebp = reinterpret_cast<uint32_t*>(ebp[0]);
        if (reinterpret_cast<uint32_t>(ebp) < MemoryLayout::KERNEL_START) {
            break;
        }
    }

    if (i >= maxStacktraceSize) {
        printLine(stringDrawer, "...");
    }
}

void BlueScreen::clear(Util::Graphic::PixelDrawer &pixelDrawer) {
    if (mode == LFB) {
        for (uint16_t i = 0; i < fbResX; i++) {
            for (uint16_t j = 0; j < fbResX; j++) {
                pixelDrawer.drawPixel(j, i, Util::Graphic::Colors::BLUE);
            }
        }
    } else {
        for (uint32_t i = 0; i < static_cast<uint32_t>(fbResX * fbResY); i++) {
            Util::Memory::Address<uint32_t> cgaMemory(fbAddress);
            cgaMemory.setShort(0x1700, i * 2);
        }

        // Set cursor shape
        Device::IoPort(0x03d4).writeByte(0x0a);
        Device::IoPort(0x03d5).writeByte(0x1c);
        Device::IoPort(0x03d4).writeByte(0x0b);
        Device::IoPort(0x03d5).writeByte(0x1b);
    }
}

void BlueScreen::print(Util::Graphic::StringDrawer &stringDrawer, const char *string) {
    if (mode == LFB) {
        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_FONT, posX * Util::Graphic::Fonts::TERMINAL_FONT.getCharWidth(),
                                posY * Util::Graphic::Fonts::TERMINAL_FONT.getCharHeight(), string, Util::Graphic::Colors::WHITE,
                                Util::Graphic::Colors::INVISIBLE);
        posX += Util::Memory::Address<uint32_t>(string).stringLength();
    } else {
        for (uint32_t i = 0; string[i] != 0; i++) {
            putCharCga(string[i]);
            posX++;
        }
    }
}

void BlueScreen::printLine(Util::Graphic::StringDrawer &stringDrawer, const char *string) {
    print(stringDrawer, string);
    posX = OFFSET_X;
    posY++;
}

void BlueScreen::printDecNumber(Util::Graphic::StringDrawer &stringDrawer, uint32_t number) {
    uint32_t div;
    for (div = 1; number / div >= 10; div *= 10);

    for (; div > 0; div /= 10) {
        char digitString[] = {static_cast<char>('0' + (number / div)), 0 };
        print(stringDrawer, digitString);

        number %= div;
    }
}

void BlueScreen::printHexNumber(Util::Graphic::StringDrawer &stringDrawer, uint32_t number) {
    uint32_t div;
    for (div = 1; number / div >= 16; div *= 16);

    print(stringDrawer, "0x");

    for (; div > 0; div /= 16) {
        char digit = static_cast<char>(number / div);
        char digitString[] = {static_cast<char>(digit < 10 ? '0' + digit : 'A' + (digit - 10)), 0 };
        print(stringDrawer, digitString);

        number %= div;
    }
}

void BlueScreen::putCharCga(char c) {
    if (posX >= fbResX || posY >= fbResY) {
        return;
    }

    uint16_t position = (posY * fbResX + posX) * 2;
    uint8_t colorAttribute = 0x17;
    Util::Memory::Address<uint32_t> cgaMemory(fbAddress);

    cgaMemory.setByte(c, position);
    cgaMemory.setByte(colorAttribute, position + 1);
}

}

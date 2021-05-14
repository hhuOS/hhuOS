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

#include <device/graphic/ColorGraphicsArray.h>
#include "MultibootTerminalProvider.h"
#include "Structure.h"

namespace Kernel::Multiboot {

MultibootTerminalProvider::MultibootTerminalProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, 0};
}

bool MultibootTerminalProvider::isAvailable() {
    const auto &frameBufferInfo = Structure::getFrameBufferInfo();
    return frameBufferInfo.type == FRAMEBUFFER_TYPE_EGA_TEXT && (frameBufferInfo.width == 80 || frameBufferInfo.width == 40) && frameBufferInfo.height == 25;
}

Util::Graphic::Terminal &MultibootTerminalProvider::initializeTerminal(Device::Graphic::TerminalProvider::ModeInfo &modeInfo) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Text mode mode has not been setup correctly by the bootloader!");
    }

    return *new Device::Graphic::ColorGraphicsArray(frameBufferInfo.width, frameBufferInfo.height);
}

void MultibootTerminalProvider::destroyTerminal(Util::Graphic::Terminal &terminal) {
    delete &terminal;
}

Util::Data::Array<MultibootTerminalProvider::ModeInfo> MultibootTerminalProvider::getAvailableModes() const {
    return supportedModes;
}

Util::Memory::String MultibootTerminalProvider::getClassName() {
    return CLASS_NAME;
}

}
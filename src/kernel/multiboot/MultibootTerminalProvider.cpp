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

#include "device/graphic/terminal/cga/ColorGraphicsAdapter.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "kernel/system/BlueScreen.h"
#include "Structure.h"
#include "MultibootTerminalProvider.h"

namespace Kernel::Multiboot {

MultibootTerminalProvider::MultibootTerminalProvider() : frameBufferInfo(Structure::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, 0};
}

bool MultibootTerminalProvider::isAvailable() {
    const auto &frameBufferInfo = Structure::getFrameBufferInfo();
    return frameBufferInfo.type == FRAMEBUFFER_TYPE_EGA_TEXT && (frameBufferInfo.width == 80 || frameBufferInfo.width == 40) && frameBufferInfo.height == 25;
}

Util::Graphic::Terminal* MultibootTerminalProvider::initializeTerminal(Device::Graphic::TerminalProvider::ModeInfo &modeInfo, const Util::Memory::String &filename) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Text mode mode has not been initializeAvailableDrives correctly by the bootloader!");
    }

    auto *terminal = new Device::Graphic::ColorGraphicsAdapter(modeInfo.columns, modeInfo.rows);
    Kernel::BlueScreen::setCgaMode(terminal->getAddress().get(), terminal->getColumns(), terminal->getRows());
    return terminal;
}

Util::Data::Array<MultibootTerminalProvider::ModeInfo> MultibootTerminalProvider::getAvailableModes() const {
    return supportedModes;
}

}
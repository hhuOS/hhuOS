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

#include <cstdint>

#include "MultibootLinearFrameBufferProvider.h"
#include "kernel/log/Logger.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/Exception.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Kernel {

Kernel::Logger MultibootLinearFrameBufferProvider::log = Kernel::Logger::get("Multiboot");

MultibootLinearFrameBufferProvider::MultibootLinearFrameBufferProvider() : frameBufferInfo(Multiboot::getFrameBufferInfo()), supportedModes(1) {
    supportedModes[0] = {static_cast<uint16_t>(frameBufferInfo.width), static_cast<uint16_t>(frameBufferInfo.height), frameBufferInfo.bpp, static_cast<uint16_t>(frameBufferInfo.pitch), 0};
}

bool MultibootLinearFrameBufferProvider::isAvailable() {
    const auto &frameBufferInfo = Multiboot::getFrameBufferInfo();
    return frameBufferInfo.type == Multiboot::RGB && frameBufferInfo.bpp >= 15;
}

Util::Graphic::LinearFrameBuffer* MultibootLinearFrameBufferProvider::initializeLinearFrameBuffer(const ModeInfo &modeInfo) {
    log.info("Checking framebuffer information, provided by the bootloader");
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "LFB mode has not been initializeAvailableControllers correctly by the bootloader!");
    }

    log.info("Framebuffer information is valid (Address: [0x%08x], Resolution: [%ux%u@%u])", frameBufferInfo.address, frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp);
    return new Util::Graphic::LinearFrameBuffer(reinterpret_cast<void*>(frameBufferInfo.address), frameBufferInfo.width, frameBufferInfo.height, frameBufferInfo.bpp, frameBufferInfo.pitch);
}

Util::Data::Array<MultibootLinearFrameBufferProvider::ModeInfo> MultibootLinearFrameBufferProvider::getAvailableModes() const {
    return supportedModes;
}

}
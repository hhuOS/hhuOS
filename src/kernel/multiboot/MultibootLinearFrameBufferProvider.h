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

#ifndef HHUOS_MULTIBOOTLINEARFRAMEBUFFERPROVIDER_H
#define HHUOS_MULTIBOOTLINEARFRAMEBUFFERPROVIDER_H

#include "device/graphic/lfb/LinearFrameBufferProvider.h"
#include "Multiboot.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"
#include "lib/util/graphic/LinearFrameBuffer.h"

namespace Kernel {
class Logger;

class MultibootLinearFrameBufferProvider : public Device::Graphic::LinearFrameBufferProvider {

public:
    /**
     * Default Constructor.
     */
    MultibootLinearFrameBufferProvider();

    /**
     * Copy Constructor.
     */
    MultibootLinearFrameBufferProvider(const MultibootLinearFrameBufferProvider &other) = delete;

    /**
     * Assignment operator.
     */
    MultibootLinearFrameBufferProvider &operator=(const MultibootLinearFrameBufferProvider &other) = delete;

    /**
     * Destructor.
     */
    ~MultibootLinearFrameBufferProvider() override = default;

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Kernel::Multiboot::MultibootLinearFrameBufferProvider")

    /**
     * Check if the framebuffer, provided by the bootloader, has the correct type and this driver can be used.
     *
     * @return true, if this driver can be used
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Overriding virtual function from LinearFrameBufferProvider.
     */
    [[nodiscard]] Util::Data::Array<ModeInfo> getAvailableModes() const override;

protected:
    /**
     * Overriding virtual function from LinearFrameBufferProvider.
     */
    Util::Graphic::LinearFrameBuffer* initializeLinearFrameBuffer(const ModeInfo &modeInfo) override;

private:

    Multiboot::FrameBufferInfo frameBufferInfo;
    Util::Data::Array<ModeInfo> supportedModes;

    static Kernel::Logger log;
};

}

#endif
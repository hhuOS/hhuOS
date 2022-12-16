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

#ifndef HHUOS_MULTIBOOTTERMINALPROVIDER_H
#define HHUOS_MULTIBOOTTERMINALPROVIDER_H

#include "Multiboot.h"
#include "device/graphic/terminal/TerminalProvider.h"
#include "lib/util/data/Array.h"
#include "lib/util/graphic/Terminal.h"
#include "lib/util/memory/String.h"
#include "lib/util/reflection/Prototype.h"

namespace Kernel {

class MultibootTerminalProvider : public Device::Graphic::TerminalProvider {

public:
    /**
     * Default Constructor.
     */
    MultibootTerminalProvider();

    /**
     * Copy Constructor.
     */
    MultibootTerminalProvider(const MultibootTerminalProvider &other) = delete;

    /**
     * Assignment operator.
     */
    MultibootTerminalProvider &operator=(const MultibootTerminalProvider &other) = delete;

    /**
     * Destructor.
     */
    ~MultibootTerminalProvider() override = default;

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Kernel::Multiboot::MultibootTerminalProvider")

    /**
     * Check if the framebuffer, provided by the bootloader, has the correct type and this driver can be used.
     *
     * @return true, if this driver can be used
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Overriding function from TerminalProvider.
     */
    [[nodiscard]] Util::Data::Array<ModeInfo> getAvailableModes() const override;

protected:
    /**
    * Overriding function from TerminalProvider.
    */
    Util::Graphic::Terminal* initializeTerminal(const ModeInfo &modeInfo) override;

private:

    Multiboot::FrameBufferInfo frameBufferInfo;
    Util::Data::Array<ModeInfo> supportedModes;
};

}

#endif
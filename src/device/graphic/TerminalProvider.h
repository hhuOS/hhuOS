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

#ifndef HHUOS_TERMINALPROVIDER_H
#define HHUOS_TERMINALPROVIDER_H

#include <util/data/Array.h>
#include <util/memory/String.h>
#include <util/graphic/Terminal.h>
#include <util/reflection/Prototype.h>

namespace Device::Graphic {

class TerminalProvider : public Util::Reflection::Prototype {

public:
    /**
     * Information about a resolution.
     *
     * @var columns Horizontal resolution
     * @var rows Vertical resolution
     * @var colorDepth Color depth in bits
     * @var modeNumber Unique number, used to identify the mode. May be ignored if not needed.
     */
    struct ModeInfo {
        uint16_t columns;
        uint16_t rows;
        uint8_t colorDepth;
        uint16_t modeNumber;

        bool operator==(const ModeInfo &other) const {
            return columns == other.columns && rows == other.rows &&
                   colorDepth == other.colorDepth && modeNumber == other.modeNumber;
        }

        bool operator!=(const ModeInfo &other) const {
            return columns != other.columns || rows != other.rows ||
                   colorDepth != other.colorDepth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Default Constructor.
     */
    TerminalProvider() = default;

    /**
     * Copy constructor.
     */
    TerminalProvider(const TerminalProvider &other) = delete;

    /**
     * Assignment operator.
     */
    TerminalProvider &operator=(const TerminalProvider &other) = delete;

    /**
     * Destructor.
     */
    ~TerminalProvider() override = default;

    /**
     * Create a terminal with a given mode info.
     * Once the terminal is not needed anymore, it's resources should be freed by calling destroyTerminal().
     *
     * @param modeInfo Information about the desired terminal resolution
     * @return A reference to the created terminal
     */
    virtual Util::Graphic::Terminal& initializeTerminal(ModeInfo &modeInfo) = 0;

    /**
     * Destroy a terminal, that has been created by initializeTerminal.
     *
     * @param lfb The terminal
     */
    virtual void destroyTerminal(Util::Graphic::Terminal &terminal) = 0;

    /**
     * Get all available graphics modes.
     *
     * @return An array, containing all available graphics modes
     */
    [[nodiscard]] virtual Util::Data::Array <ModeInfo> getAvailableModes() const = 0;

    /**
     * Get the amount of video memory, that the device has.
     * Returns 0 if the amount is not known.
     *
     * @return The amount of video memory
     */
    [[nodiscard]] virtual uint32_t getVideoMemorySize() const;

    /**
     * Get the name of the device's vendor.
     * Returns "Unknown", if the vendor is not known.
     *
     * @return The vendor's name
     */
    [[nodiscard]] virtual Util::Memory::String getVendorName() const;

    /**
     * Get the device's name.
     * Returns "Unknown", if the name is not known.
     *
     * @return The device's name
     */
    [[nodiscard]] virtual Util::Memory::String getDeviceName() const;

    /**
     * Search for a graphics mode, that suits the given parameters best.
     *
     * @param columns The desired width
     * @param rows The desired height
     * @param colorDepth The desired color depth
     * @return The found ModeInfo struct
     */
    [[nodiscard]] ModeInfo searchMode(uint16_t columns, uint16_t rows, uint8_t colorDepth) const;

    /**
     * Overriding function from Prototype.
     */
    [[nodiscard]] Util::Memory::String getClassName() override = 0;
};

}

#endif

/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_LINEARFRAMEBUFFERPROVIDER_H
#define HHUOS_LINEARFRAMEBUFFERPROVIDER_H

#include <util/data/Array.h>
#include <util/memory/String.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/reflection/Prototype.h>

namespace Device::Graphic {

class LinearFrameBufferProvider : public Util::Reflection::Prototype {

public:
    /**
     * Information about a resolution.
     *
     * @var resolutionX Horizontal resolution
     * @var resolutionY Vertical resolution
     * @var colorDepth Color depth in bits
     * @var pith The pitch
     * @var modeNumber Unique number, used to identify the mode. May be ignored if not needed.
     */
    struct ModeInfo {
        uint16_t resolutionX;
        uint16_t resolutionY;
        uint8_t colorDepth;
        uint16_t pitch;
        uint16_t modeNumber;

        bool operator==(const ModeInfo &other) const {
            return resolutionX == other.resolutionX && resolutionY == other.resolutionY &&
                   pitch == other.pitch && colorDepth == other.colorDepth && modeNumber == other.modeNumber;
        }

        bool operator!=(const ModeInfo &other) const {
            return resolutionX != other.resolutionX || resolutionY != other.resolutionY ||
                    pitch != other.pitch || colorDepth != other.colorDepth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Default Constructor.
     */
    LinearFrameBufferProvider() = default;

    /**
     * Copy constructor.
     */
    LinearFrameBufferProvider(const LinearFrameBufferProvider &other) = delete;

    /**
     * Assignment operator.
     */
    LinearFrameBufferProvider &operator=(const LinearFrameBufferProvider &other) = delete;

    /**
     * Destructor.
     */
    virtual ~LinearFrameBufferProvider() = default;

    /**
     * Create a linear frame buffer with a given mode info.
     * Once the linear frame buffer is not needed anymore, it's resources should be freed by calling destroyLinearFrameBuffer().
     *
     * @param modeInfo Information about the desired frame buffer resolution
     * @return A pointer to the linear frame buffer
     */
    virtual Util::Graphic::LinearFrameBuffer& initializeLinearFrameBuffer(ModeInfo &modeInfo) = 0;

    /**
     * Destroy a linear frame buffer, that has been created by initializeLinearFrameBuffer.
     *
     * @param lfb The linear frame buffer
     */
    virtual void destroyLinearFrameBuffer(Util::Graphic::LinearFrameBuffer &lfb) = 0;

    /**
     * Get all available graphics modes.
     *
     * @return An array, containing all available graphics modes
     */
    [[nodiscard]] virtual Util::Data::Array<ModeInfo> getAvailableModes() const = 0;

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
     * @param resolutionX The desired width
     * @param resolutionY The desired height
     * @param colorDepth The desired color depth
     * @return The found ModeInfo struct
     */
    [[nodiscard]] ModeInfo searchMode(uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth) const;

    /**
     * Overriding function from Prototype.
     */
    [[nodiscard]] Util::Memory::String getClassName() override = 0;
};

}

#endif

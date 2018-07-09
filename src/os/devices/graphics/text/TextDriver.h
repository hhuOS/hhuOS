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

#ifndef __TextDriver_include__
#define __TextDriver_include__

#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "lib/OutputStream.h"
#include "lib/graphic/Colors.h"
#include "lib/util/ArrayList.h"

#include <cstdint>
#include <lib/String.h>

/**
 * Interface for graphics devices, that can display text.
 *
 * Implementations must not show a cursor. The OS will automatically print a cursor at the current position.
 */
class TextDriver : public OutputStream, public KernelService {

protected:
    uint16_t columns = 0;
    uint16_t rows = 0;
    uint8_t depth = 0;

public:
    /**
     * Information about a resolution.
     *
     * @var columns Amount of columns
     * @var rows Amount rows
     * @var depth Color-depth in bits
     * @var modeNumber Unique number, used to identify the mode.
     *      May be ignored if not needed.
     */
    struct TextResolution {
        uint16_t columns;
        uint16_t rows;
        uint8_t depth;
        uint16_t modeNumber;

        bool operator==(const TextResolution &other) const {
            return columns == other.columns && rows == other.rows &&
                   depth == other.depth && modeNumber == other.modeNumber;
        }

        bool operator!=(const TextResolution &other) const {
            return columns != other.columns || rows != other.rows ||
                   depth != other.depth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Constructor.
     */
    TextDriver() = default;

    /**
     * Copy-constructor.
     */
    TextDriver(const TextDriver &copy) = delete;

    /**
     * Destructor.
     */
    ~TextDriver() override = default;

    /**
     * Get the amount columns.
     */
    uint16_t getColumnCount();

    /**
     * Get the amount of rows.
     */
    uint16_t getRowCount();

    /**
     * Get the color-depth.
     */
    uint8_t getDepth();

    /**
     * Overriding virtual function from OutputStream.
     */
    void flush() override;

    /**
     * Initialize the screen with a desired resolution.
     *
     * @param columns Desired amount of columns
     * @param rows Desired amount of rows
     * @param depth Desired color-depth
     * @return true, on success
     */
    bool init(uint16_t columns, uint16_t rows, uint8_t depth);

    /**
     * Check, whether the graphics device is available.
     *
     * @return true, if the hardware is available
     */
    virtual bool isAvailable() = 0;

    /**
     * Get all available resolutions.
     *
     * @return A List, containing all available resolutions
     */
    virtual Util::Array<TextResolution> getTextResolutions() = 0;

    /**
     * Get the name of the device's vendor.
     * Return "Unknown", if the vendor is not known.
     */
    virtual String getVendorName() = 0;

    /**
     * Get the device's name.
     * Return "Unknown", if the name is not known.
     */
    virtual String getDeviceName() = 0;

    /**
     * Get the amount of video memory, that the device has.
     */
    virtual uint32_t getVideoMemorySize() = 0;

    /**
     * Set the cursor position.
     *
     * @param x The column
     * @param y The row
     */
    virtual void setpos(uint16_t x, uint16_t y) = 0;

    /**
     * Get the cursor position.
     *
     * @param x A reference to the variable, that the column will be written to
     * @param y A reference to the variable, that the row will be written to
     */
    virtual void getpos(uint16_t& x, uint16_t& y) = 0;

    /**
     * Print a character at a given position.
     *
     * @param x The column
     * @param y The row
     * @param c The character
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    virtual void show(uint16_t x, uint16_t y, char c, Color fgColor = Colors::HHU_GRAY, Color bgColor = Colors::INVISIBLE) = 0;

    /**
     * Print a char at the current position.
     *
     * @param c The char
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    virtual void putc(char c, Color fgColor = Colors::HHU_GRAY, Color bgColor = Colors::INVISIBLE) = 0;

    /**
     * Print a string at the current position.
     *
     * @param s The string
     * @param n The string's length
     * @param fgColor The foreground color
     * @param bgColor The background color
     */
    virtual void puts(const char *s, uint32_t n, Color fgColor = Colors::HHU_GRAY, Color bgColor = Colors::INVISIBLE) = 0;

    /**
     * Scrolls the entire screen up by one row.
     * After that, the position should be set to the first column of the last row.
     */
    virtual void scrollup() = 0;

    /**
     * Clears the entire screen.
     * After that, the postions should be set to the first column of the first row.
     */
    virtual void clear() = 0;

private:
    /**
     * Initialize the screen with a resolution.
     * The resolution is taken from the list returned by getTextResolutions().
     * This function only gets called by TextDriver::init().
     *
     * An implementation must not call clear()! This will be done automatically by init()!
     *
     * @param resolution The resolution
     * @return true, on success
     */
    virtual bool setResolution(TextResolution resolution) = 0;

    /**
     * Takes a desired resolution and searches for the most fitting one from all available resolutions.
     *
     * @param columns Desired amount of columns
     * @param rows Desired amount of rows
     * @param depth Desired color-depth
     * @return The found resolution
     */
    TextResolution findBestResolution(uint16_t columns, uint16_t rows, uint8_t depth);
};

#endif
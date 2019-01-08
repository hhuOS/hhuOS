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

#ifndef __LinearFrameBuffer_include__
#define __LinearFrameBuffer_include__

#include "lib/graphic/Colors.h"
#include "devices/graphics/text/fonts/Fonts.h"
#include "kernel/KernelService.h"
#include "lib/util/ArrayList.h"

#include <cstdint>
#include <lib/String.h>

/**
 * Interface for graphics devices, that can be accessed via a linear frame buffer.
 */
class LinearFrameBuffer {

private:
    /**
     * Draw a monochrom bitmap at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param width The bitmap's width
     * @param height The bitmap's height
     * @param fgColor The foreground color
     * @param bgColor The background color
     * @param bitmap The bitmap's data.
     */
    void drawMonoBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color fgColor, Color bgColor, uint8_t *bitmap);

    static const constexpr char *NAME = "LinearFrameBuffer";
    static const constexpr char *DEVICE_NAME = "Generic Graphics Adapter";
    static const constexpr char *VENDOR_NAME = "Unknown";

protected:
    uint16_t xres = 0;
    uint16_t yres = 0;
    uint8_t bpp = 0;

    uint16_t pitch = 0;

    bool doubleBuffered = false;
    uint8_t *doubleBuffer = nullptr;

    uint8_t *hardwareBuffer = nullptr;

public:
    /**
     * Information about a resolution.
     *
     * @var resX Horizontal resolution
     * @var resY Vertical resolution
     * @var depth Color-depth in bits
     * @var modeNumber Unique number, used to identify the mode.
     *      May be ignored of not needed.
     */
    struct LfbResolution {
        uint16_t resX;
        uint16_t resY;
        uint8_t depth;
        uint16_t modeNumber;

        bool operator==(const LfbResolution &other) const {
            return resX == other.resX && resY == other.resY &&
                   depth == other.depth && modeNumber == other.modeNumber;
        }

        bool operator!=(const LfbResolution &other) const {
            return resX != other.resX || resY != other.resY ||
                   depth != other.depth || modeNumber != other.modeNumber;
        }
    };

    /**
     * Constructor.
     */
    LinearFrameBuffer(void *address, uint16_t xres, uint16_t yres, uint8_t bpp, uint16_t pitch);

    /**
     * Copy-constructor.
     */
    LinearFrameBuffer(const LinearFrameBuffer &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~LinearFrameBuffer() = default;

    /**
     * Initialize the screen with a desired resolution.
     *
     * @param resX Desired horizontal resolution
     * @param resY Desired vertical resolution
     * @param depth Desired color-depth
     * @return true, on success
     */
    bool init(uint16_t resX, uint16_t resY, uint8_t depth);

    /**
     * Get the driver's name (e.g. 'CgaGraphics', 'VesaGraphics', etc.).
     */
    virtual String getName() ;

    /**
     * Check, whether the graphics device is available.
     *
     * @return true, if the hardware is available
     */
    virtual bool isAvailable();

    /**
     * Get all available resolutions.
     *
     * @return A List, containing all available resolutions
     */
    virtual Util::Array<LfbResolution> getLfbResolutions();

    /**
     * Get the name of the device's vendor.
     * Return "Unknown", if the vendor is not known.
     */
    virtual String getVendorName();

    /**
     * Get the device's name.
     * Return "Unknown", if the name is not known.
     */
    virtual String getDeviceName();

    /**
     * Get the amount of video memory, that the device has.
     */
    virtual uint32_t getVideoMemorySize();

    /**
     * Get the horizontal resolution.
     */
    uint16_t getResX();

    /**
     * Get the vertial resolution.
     */
    uint16_t getResY();

    /**
     * Get the color-depth.
     */
    uint8_t getDepth();

    /**
     * Draw a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color The color
     */
    virtual void drawPixel(uint16_t x, uint16_t y, Color color);

    /**
     * Read the color of a pixel at a given position.
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param color A reference to the variable, that the pixel's color will be written to
     */
    virtual void readPixel(uint16_t x, uint16_t y, Color &color);

    /**
     * Draw a line.
     *
     * @param x1 x-coordinate of start-point
     * @param y1 y-coordinate of start-point
     * @param x2 x-coordinate of end-point
     * @param y2 y-coordinate of end-point
     * @param color The color
     */
    virtual void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color);

    /**
     * Draw the outline of a rectangle.
     *
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param width The width
     * @param height The height
     * @param color The color
     */
    virtual void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

    /**
     * Draw a filled rectangle.
     *
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param width The width
     * @param height The height
     * @param color The color
     */
    virtual void fillRect(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, Color color);

    /**
     * Draw a filled circle.
     *
     * @param x x-coordinate of the circle's center
     * @param y y-coordinate of the circle's center
     * @param r The radius
     * @param color The color
     */
    virtual void fillCircle(uint16_t x, uint16_t y, uint16_t r, Color color);

    /**
     * Draw a char.
     *
     * @param fnt The font
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param fgColor The foreground color
     * @param bgColor The background color
     * @param c The char
     */
    virtual void drawChar(Font &fnt, uint16_t x, uint16_t y, char c, Color fgColor, Color bgColor);

    /**
     * Draw a null-terminated string.
     *
     * @param fnt The font
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param fgColor The foreground color
     * @param bgColor The background color
     * @param s The string
     */
    virtual void drawString(Font &fnt, uint16_t x, uint16_t y, const char *s, Color fgColor, Color bgColor);

    /**
     * Draw a 32-bit RGBA-sprite. The sprite data can be generated with GIMP.
     *
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param width The width
     * @param height The height
     * @param data The sprite-data
     */
    virtual void drawSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int32_t *data);

    /**
     * Clear the entire screen.
     */
    virtual void clear();

    /**
     * Draw a line with relative coordinates (0-100).
     *
     * @param x1_p x-coordinate of start-point
     * @param y1_p y-coordinate of start-point
     * @param x2_p x-coordinate of end-point
     * @param y2_p y-coordinate of end-point
     * @param color The color
     */
    void placeLine(uint16_t x1_p, uint16_t y1_p, uint16_t x2_p, uint16_t y2_p, Color color);

    /**
     * Draw the outline of a rectangle with relative coordinates and size (0-100).
     *
     * @param x_p x-coordinate of upper-left corner
     * @param y_p y-coordinate of upper-left corner
     * @param width_p The width
     * @param height_p The height
     * @param color The color
     */
    void placeRect(uint16_t x_p, uint16_t y_p, uint16_t width_p, uint16_t height_p, Color color);

    /**
     * Draw a filled rectangle with relative coordinates and size (0-100).
     *
     * @param x_p x-coordinate of upper-left corner
     * @param y_p y-coordinate of upper-left corner
     * @param width_p The width
     * @param height_p The height
     * @param color The color
     */
    void placeFilledRect(uint16_t x_p, uint16_t y_p, uint16_t width_p, uint16_t height_p, Color color);

    /**
     * Draw a filled circle with relative coordinates and size (0-100).
     *
     * @param x_p x-coordinate of the circle's center
     * @param y_p y-coordinate of the circle's center
     * @param r_p The radius
     * @param color The color
     */
    void placeFilledCircle(uint16_t x_p, uint16_t y_p, uint16_t r_p, Color color);

    /**
     * Draw a null-terminated string with relative coordinates (0-100).
     *
     * @param fnt The font
     * @param x_p x-coordinate of upper-left corner
     * @param y_p y-coordinate of upper-left corner
     * @param fgColor The foreground color
     * @param bgColor The background color
     * @param s The string
     */
    void placeString(Font &fnt, uint16_t x_p, uint16_t y_p, const char *s, Color fgColor, Color bgColor = Colors::INVISIBLE);

    /**
     * Draw a 32-bit RGBA-sprite relative coordinates (0-100). The sprite data can be generated with GIMP.
     *
     * @param x x-coordinate of upper-left corner
     * @param y y-coordinate of upper-left corner
     * @param width The width
     * @param height The height
     * @param data The sprite-data
     */
    void placeSprite(uint16_t x_p, uint16_t y_p, uint16_t width, uint16_t height, int32_t *data);

    /**
     * Enable double-buffering.
     */
    virtual void enableDoubleBuffering();

    /**
     * Disable double-buffering.
     */
    virtual void disableDoubleBuffering();

    /**
     * Check, if double-buffering is enabled.
     */
    virtual bool isDoubleBuffered();

    /**
     * Flush the buffer, when double-buffering is enabled.
     */
    virtual void show();

private:
    /**
     * Initialize the screen with a resolution.
     * The resolution is taken from the list returned by getLfbResolutions().
     * This function only gets called by LinearFrameBuffer::init().
     *
     * An implementation must not call clear()! This will be done automatically by init()!
     * An implementation must not set the variables xres, yres and bpp! This will be done automatically by init()!
     *
     * @param resolution The resolution
     * @return true, on success
     */
    virtual bool setResolution(LfbResolution resolution);

    /**
     * Takes a desired resolution and searches for the most fitting one from all available resolutions.
     *
     * @param resX Desired amount of columns
     * @param resY Desired amount of rows
     * @param depth Desired color-depth
     * @return The found resolution
     */
    LfbResolution findBestResolution(uint16_t resX, uint16_t resY, uint8_t depth);

    /**
     * Reallocate the buffer, that is used for double-buffering.
     */
    void reallocBuffer();
};

#endif

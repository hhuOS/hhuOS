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

#ifndef HHUOS_GRAPHICS2D_H
#define HHUOS_GRAPHICS2D_H

#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"

namespace Util::Game {

class Graphics2D {

public:
    /**
     * Default Constructor.
     */
    explicit Graphics2D(const Util::Graphic::LinearFrameBuffer &lfb);

    /**
     * Copy Constructor.
     */
    Graphics2D(const Graphics2D &other) = delete;

    /**
     * Assignment operator.
     */
    Graphics2D &operator=(const Graphics2D &other) = delete;

    /**
     * Destructor.
     */
    ~Graphics2D() = default;

    void drawLine(double x1, double y1, double x2, double y2) const;

    void drawPolygon(double *x, double *y, uint32_t corners) const;

    void drawRectangle(double x1, double y1, double x2, double y2) const;

    void drawString(double x, double y, const char *string) const;

    void drawString(double x, double y, const Util::Memory::String &string) const;

    void drawStringSmall(double x, double y, const char *string) const;

    void drawStringSmall(double x, double y, const Util::Memory::String &string) const;

    void show() const;

    void setColor(const Graphic::Color &color);

    [[nodiscard]] Graphic::Color getColor() const;

private:

    void drawString(const Graphic::Font &font, double x, double y, const char *string) const;

    const Graphic::BufferedLinearFrameBuffer lfb;
    const Graphic::PixelDrawer pixelDrawer;
    const Graphic::LineDrawer lineDrawer;
    const Graphic::StringDrawer stringDrawer;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;

    Graphic::Color color = Graphic::Colors::WHITE;
};

}

#endif

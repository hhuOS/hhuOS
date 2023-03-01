/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#ifndef HHUOS_GRAPHICS2D_H
#define HHUOS_GRAPHICS2D_H

#include <cstdint>

#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector2D.h"

namespace Util {

namespace Graphic {
class Font;
class LinearFrameBuffer;
class Image;
}  // namespace Graphic

namespace Game {
class Game;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class Graphics2D {

public:
    /**
     * Constructor.
     */
    explicit Graphics2D(const Util::Graphic::LinearFrameBuffer &lfb, Game &game);

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

    void drawLine(const Math::Vector2D &from, const Math::Vector2D &to) const;

    void drawPolygon(const Array<Math::Vector2D> &vertices) const;

    void drawSquare(const Math::Vector2D &position, double size) const;

    void drawRectangle(const Math::Vector2D &position, double width, double height) const;

    void fillSquare(const Math::Vector2D &position, double size) const;

    void fillRectangle(const Math::Vector2D &position, double width, double height) const;

    void drawString(const Math::Vector2D &position, const char *string) const;

    void drawString(const Math::Vector2D &position, const String &string) const;

    void drawStringSmall(const Math::Vector2D &position, const char *string) const;

    void drawStringSmall(const Math::Vector2D &position, const String &string) const;

    void drawImage(const Math::Vector2D &position, const Graphic::Image &image, bool flipX = false) const;

    void clear(const Graphic::Color &color);

    void show() const;

    void setColor(const Graphic::Color &color);

    [[nodiscard]] Graphic::Color getColor() const;

    void saveCurrentStateAsBackground();

    [[nodiscard]] Math::Vector2D getAbsoluteResolution() const;

private:

    void drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const;

    Game &game;

    const Graphic::BufferedLinearFrameBuffer lfb;
    const Graphic::PixelDrawer pixelDrawer;
    const Graphic::LineDrawer lineDrawer;
    const Graphic::StringDrawer stringDrawer;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;

    uint8_t *backgroundBuffer = nullptr;

    Graphic::Color color = Graphic::Colors::WHITE;
};

}

#endif

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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_GRAPHICS_H
#define HHUOS_GRAPHICS_H

#include <cstdint>

#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3D.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math

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

class Graphics {

public:
    /**
     * Constructor.
     */
    explicit Graphics(const Util::Graphic::LinearFrameBuffer &lfb, Game &game);

    /**
     * Copy Constructor.
     */
    Graphics(const Graphics &other) = delete;

    /**
     * Assignment operator.
     */
    Graphics &operator=(const Graphics &other) = delete;

    /**
     * Destructor.
     */
    ~Graphics() = default;

    /***** Basic functions to draw directly on the screen ******/

    void drawLine(const Math::Vector2D &from, const Math::Vector2D &to) const;

    void drawPolygon(const Array<Math::Vector2D> &vertices) const;

    void drawRectangle(const Math::Vector2D &position, double width, double height) const;

    void drawSquare(const Math::Vector2D &position, double size) const;

    void fillRectangle(const Math::Vector2D &position, double width, double height) const;

    void fillSquare(const Math::Vector2D &position, double size) const;

    void drawString(const Math::Vector2D &position, const char *string) const;

    void drawString(const Math::Vector2D &position, const String &string) const;

    void drawStringSmall(const Math::Vector2D &position, const char *string) const;

    void drawStringSmall(const Math::Vector2D &position, const String &string) const;

    /***** 2D drawing functions, respecting the camera position *****/

    void drawLine2D(const Math::Vector2D &from, const Math::Vector2D &to) const;

    void drawPolygon2D(const Array<Math::Vector2D> &vertices) const;

    void drawSquare2D(const Math::Vector2D &position, double size) const;

    void drawRectangle2D(const Math::Vector2D &position, double width, double height) const;

    void fillSquare2D(const Math::Vector2D &position, double size) const;

    void fillRectangle2D(const Math::Vector2D &position, double width, double height) const;

    void drawString2D(const Math::Vector2D &position, const char *string) const;

    void drawString2D(const Math::Vector2D &position, const String &string) const;

    void drawStringSmall2D(const Math::Vector2D &position, const char *string) const;

    void drawStringSmall2D(const Math::Vector2D &position, const String &string) const;

    void drawImage2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX = false) const;

    /***** 2D drawing functions *****/

    [[nodiscard]] Math::Vector2D projectPoint(const Math::Vector3D &v, const Math::Vector3D &camT, const Math::Vector3D &camRr) const;

    void drawLine3D(const Math::Vector3D &from, const Math::Vector3D &to) const;

    void drawModel(const Array<Math::Vector3D> &vertices, const Array<Math::Vector2D> &edges) const;


    /***** Miscellaneous *****/

    [[nodiscard]] uint8_t getCharWidth() const;

    [[nodiscard]] uint8_t getCharHeight() const;

    [[nodiscard]] uint8_t getCharWidthSmall() const;

    [[nodiscard]] uint8_t getCharHeightSmall() const;

    void clear(const Graphic::Color &color = Util::Graphic::Colors::BLACK);

    void show() const;

    void update();

    void setColor(const Graphic::Color &color);

    [[nodiscard]] Graphic::Color getColor() const;

    void saveCurrentStateAsBackground();

    void clearBackground();

private:

    void drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const;

    void drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const;

    Game &game;

    const Graphic::BufferedLinearFrameBuffer lfb;
    const Graphic::PixelDrawer pixelDrawer;
    const Graphic::LineDrawer lineDrawer;
    const Graphic::StringDrawer stringDrawer;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;

    const Graphic::Font &font = Graphic::Fonts::TERMINAL_FONT;
    const Graphic::Font &fontSmall = Graphic::Fonts::TERMINAL_FONT_SMALL;

    Math::Vector3D cameraPosition{};
    Math::Vector3D cameraRotation{};

    uint8_t *backgroundBuffer = nullptr;

    Graphic::Color color = Graphic::Colors::WHITE;
};

}

#endif

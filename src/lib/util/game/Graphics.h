/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * It has been enhanced with image rotation and scaling capabilities during a bachelor's thesis by Abdulbasir Gümüs
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_GRAPHICS_H
#define HHUOS_GRAPHICS_H

#include <stdint.h>

#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3D.h"
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

class Graphics {

friend class Engine;

public:
    /**
     * Constructor.
     */
    explicit Graphics(const Util::Graphic::LinearFrameBuffer &lfb, Game &game, double scaleFactor);

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

    void drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const;

    void drawString(const Graphic::Font &font, const Math::Vector2D &position, const String &string) const;

    /***** 2D drawing functions, respecting the camera position *****/

    void drawLine2D(const Math::Vector2D &from, const Math::Vector2D &to) const;

    void drawPolygon2D(const Array<Math::Vector2D> &vertices) const;

    void drawSquare2D(const Math::Vector2D &position, double size) const;

    void drawRectangle2D(const Math::Vector2D &position, double width, double height) const;

    void fillSquare2D(const Math::Vector2D &position, double size) const;

    void fillRectangle2D(const Math::Vector2D &position, double width, double height) const;

    void drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const;

    void drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const String &string) const;

    void drawImage2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX = false, double alpha = 1, const Math::Vector2D &scale = Util::Math::Vector2D(1, 1), double rotationAngle = 0) const;

    /***** 2D drawing functions *****/

    [[nodiscard]] static Math::Vector2D projectPoint(const Math::Vector3D &vertex, const Math::Vector3D &cameraPosition, const Math::Vector3D &cameraRotation) ;

    void drawLine3D(const Math::Vector3D &from, const Math::Vector3D &to);

    void drawModel(const Array<Math::Vector3D> &vertices, const Array<Math::Vector2D> &edges);

    /***** Miscellaneous *****/

    void clear(const Graphic::Color &color = Util::Graphic::Colors::BLACK);

    void show() const;

    void update();

    void setColor(const Graphic::Color &color);

    [[nodiscard]] Graphic::Color getColor() const;

    void saveCurrentStateAsBackground();

    void clearBackground();

private:

    void drawImageDirect2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha) const;

    void drawImageScaled2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, const Util::Math::Vector2D &scale) const;

    void drawImageRotated2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, double rotationAngle) const;

    void drawImageScaledAndRotated2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, const Util::Math::Vector2D &scale, double rotationAngle) const;

    void resetCounters();

    Game &game;

    const Graphic::BufferedLinearFrameBuffer bufferedLfb;
    const Graphic::PixelDrawer pixelDrawer;
    const Graphic::LineDrawer lineDrawer;
    const Graphic::StringDrawer stringDrawer;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;

    Math::Vector3D cameraPosition{};
    Math::Vector3D cameraRotation{};

    uint8_t *backgroundBuffer = nullptr;

    uint32_t edgeCounter = 0;
    uint32_t drawnEdgeCounter = 0;

    Graphic::Color color = Graphic::Colors::WHITE;

    static const constexpr double FIELD_OF_VIEW = 1.3;
};

}

#endif

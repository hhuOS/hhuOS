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
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_GRAPHICS_H
#define HHUOS_GRAPHICS_H

#include <stdint.h>

#include "lib/tinygl/include/zbuffer.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/math/Vector2.h"
#include "Camera.h"
#include "lib/tinygl/include/GL/gl.h"

namespace Util {

namespace Graphic {
class Font;
class LinearFrameBuffer;
class Image;
}  // namespace Graphic

namespace Game {
class Game;
namespace D3 {
class Model;
}  // namespace D3
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

    void drawLineDirectAbsolute(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const;

    void drawRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const;

    void drawSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const;

    void fillRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const;

    void fillSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const;

    void drawStringDirectAbsolute(const Graphic::Font &font, uint16_t posX, uint16_t posY, const char *string) const;

    void drawStringDirectAbsolute(const Graphic::Font &font, uint16_t posX, uint16_t posY, const String &string) const;

    void drawLineDirect(const Math::Vector2<double> &from, const Math::Vector2<double> &to) const;

    void drawRectangleDirect(const Math::Vector2<double> &position, double width, double height) const;

    void drawSquareDirect(const Math::Vector2<double> &position, double size) const;

    void fillRectangleDirect(const Math::Vector2<double> &position, double width, double height) const;

    void fillSquareDirect(const Math::Vector2<double> &position, double size) const;

    void drawStringDirect(const Graphic::Font &font, const Math::Vector2<double> &position, const char *string) const;

    void drawStringDirect(const Graphic::Font &font, const Math::Vector2<double> &position, const String &string) const;

    /***** 2D drawing functions, respecting the camera position *****/

    void drawLine2D(const Math::Vector2<double> &from, const Math::Vector2<double> &to) const;

    void drawPolygon2D(const Array<Math::Vector2<double>> &vertices) const;

    void drawSquare2D(const Math::Vector2<double> &position, double size) const;

    void drawRectangle2D(const Math::Vector2<double> &position, double width, double height) const;

    void fillSquare2D(const Math::Vector2<double> &position, double size) const;

    void fillRectangle2D(const Math::Vector2<double> &position, double width, double height) const;

    void drawString2D(const Graphic::Font &font, const Math::Vector2<double> &position, const char *string) const;

    void drawString2D(const Graphic::Font &font, const Math::Vector2<double> &position, const String &string) const;

    void drawImage2D(const Math::Vector2<double> &position, const Graphic::Image &image, bool flipX = false, double alpha = 1, const Math::Vector2<double> &scale = Util::Math::Vector2<double>(1, 1), double rotationAngle = 0) const;

    /***** 3D drawing functions *****/

    void drawModel(const D3::Model &model);

    /***** Miscellaneous *****/

    void initializeGl();

    void closeGl();

    [[nodiscard]] bool glEnabled() const;

    void clear(const Graphic::Color &color = Util::Graphic::Colors::BLACK);

    void show() const;

    void update();

    void setColor(const Graphic::Color &color);

    [[nodiscard]] Graphic::Color getColor() const;

    void saveCurrentStateAsBackground();

    void clearBackground();

private:

    void drawImageDirect2D(const Math::Vector2<double> &position, const Graphic::Image &image, bool flipX, double alpha) const;

    void drawImageScaled2D(const Math::Vector2<double> &position, const Graphic::Image &image, bool flipX, double alpha, const Util::Math::Vector2<double> &scale) const;

    void drawImageRotated2D(const Math::Vector2<double> &position, const Graphic::Image &image, bool flipX, double alpha, double rotationAngle) const;

    void drawImageScaledAndRotated2D(const Math::Vector2<double> &position, const Graphic::Image &image, bool flipX, double alpha, const Util::Math::Vector2<double> &scale, double rotationAngle) const;

    static void gluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar);

    Game &game;

    const Graphic::BufferedLinearFrameBuffer bufferedLfb;
    ZBuffer *glBuffer = nullptr;
    uint8_t *backgroundBuffer = nullptr;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;

    Graphic::Color color = Graphic::Colors::WHITE;

    const Math::Vector3<double> worldUpVecotor = Math::Vector3<double>(0, 1, 0);
    Camera camera;
};

}

#endif

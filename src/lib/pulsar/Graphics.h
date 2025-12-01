/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_PULSAR_GRAPHICS_H
#define HHUOS_LIB_PULSAR_GRAPHICS_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"
#include "util/graphic/Image.h"
#include "util/math/Vector3.h"
#include "util/math/Vector2.h"
#include "pulsar/Camera.h"
#include "pulsar/3d/Texture.h"
#include "tinygl/include/GL/gl.h"
#include "tinygl/include/zbuffer.h"

namespace Pulsar {
namespace D3 {
class Model;
}
}

namespace Pulsar {

/// Provides graphics rendering capabilities for 2D and 3D objects.
/// It supports drawing basic shapes, text, images, and 3D models.
/// 3D drawing functions use TinyGL (https://github.com/C-Chads/tinygl/tree/main) for rendering.
/// Basic 2D drawing functions can also be made directly to the screen (e.g., for UI elements).
/// Not all functions are available in both 2D and 3D scenes. See the documentation of each function for details.
/// It is instantiated by the engine and passed to all drawing functions.
/// 3D operations can be sped up using display lists (see `startList3D()`, `endList3D()` and `drawList3D()`).
class Graphics {

public:
    /// Create a new graphics instance with the given linear frame buffer and scale factor.
    /// The scale factor determines the internal render resolution, which is scaled to the actual screen resolution.
    explicit Graphics(const Util::Graphic::LinearFrameBuffer &lfb, float scaleFactor);

    /// The graphics class is not copyable, so the copy constructor is deleted.
    Graphics(const Graphics &other) = delete;

    /// The graphics class is not copyable, so the assignment operator is deleted.
    Graphics &operator=(const Graphics &other) = delete;

    /// Destroy the graphics instance and free all associated resources.
    ~Graphics();

    /***** Basic functions to draw directly on the screen ******/

    /// Draw a line directly on the screen using absolute pixel coordinates.
    /// Only works with 2D scenes.
    void drawLineDirectAbsolute(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const;

    /// Draw the outline of a rectangle directly on the screen using absolute pixel coordinates.
    /// Only works with 2D scenes.
    void drawRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const;

    /// Draw the outline of a square directly on the screen using absolute pixel coordinates.
    /// Only works with 2D scenes.
    void drawSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const;

    /// Draw a filled rectangle directly on the screen using absolute pixel coordinates.
    /// Only works with 2D scenes.
    void fillRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const;

    /// Draw a filled square directly on the screen using absolute pixel coordinates.
    /// Works for both 2D and 3D scenes.
    void fillSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const;

    /// Draw a string directly on the screen using absolute pixel coordinates.
    /// The used font size can be retrieved via `FONT_SIZE`.
    /// Works for both 2D and 3D scenes.
    void drawStringDirectAbsolute(uint16_t posX, uint16_t posY, const char *string) const;

    /// Draw a string directly on the screen using absolute pixel coordinates.
    /// The used font size can be retrieved via `FONT_SIZE`.
    /// Works for both 2D and 3D scenes.
    void drawStringDirectAbsolute(uint16_t posX, uint16_t posY, const Util::String &string) const;

    /// Draw a line directly on the screen using relative coordinates (-1 to 1).
    /// Works for both 2D and 3D games.
    void drawLineDirect(const Util::Math::Vector2<float> &from, const Util::Math::Vector2<float> &to) const;

    /// Draw the outline of a rectangle directly on the screen using relative coordinates (-1 to 1).
    /// Works for both 2D and 3D games.
    void drawRectangleDirect(const Util::Math::Vector2<float> &position, const Util::Math::Vector2<float> &size) const;

    /// Draw the outline of a square directly on the screen using relative coordinates (-1 to 1).
    /// Works for both 2D and 3D games.
    void drawSquareDirect(const Util::Math::Vector2<float> &position, float size) const;

    /// Draw a filled rectangle directly on the screen using relative coordinates (-1 to 1).
    /// Works for both 2D and 3D games.
    void fillRectangleDirect(const Util::Math::Vector2<float> &position, const Util::Math::Vector2<float> &size) const;

    /// Draw a filled square directly on the screen using relative coordinates (-1 to 1).
    /// Works for both 2D and 3D games.
    void fillSquareDirect(const Util::Math::Vector2<float> &position, float size) const;

    /// Draw a string directly on the screen using relative coordinates (-1 to 1).
    /// The used font size can be retrieved via `FONT_SIZE`.
    /// Works for both 2D and 3D games.
    void drawStringDirect(const Util::Math::Vector2<float> &position, const char *string) const;

    /// Draw a string directly on the screen using relative coordinates (-1 to 1).
    /// The used font size can be retrieved via `FONT_SIZE`.
    /// Works for both 2D and 3D games.
    void drawStringDirect(const Util::Math::Vector2<float> &position, const Util::String &string) const;

    /***** 2D drawing functions, respecting the camera position *****/

    /// Draw a line in 2D space. Works only in 2D scenes.
    void drawLine2D(const Util::Math::Vector2<float> &from, const Util::Math::Vector2<float> &to) const;

    /// Draw the outline of a polygon in 2D space. Works only in 2D scenes.
    /// The last vertex is automatically connected to the first vertex.
    /// The position is added to each vertex for drawing.
    void drawPolygon2D(const Util::Math::Vector2<float> &position,
        const Util::Array<Util::Math::Vector2<float>> &vertices) const;

    /// Draw the outline of a square in 2D space. Works only in 2D scenes.
    void drawSquare2D(const Util::Math::Vector2<float> &position, float size) const;

    /// Draw the outline of a rectangle in 2D space. Works only in 2D scenes.
    void drawRectangle2D(const Util::Math::Vector2<float> &position, const Util::Math::Vector2<float> &size) const;

    /// Draw a filled square in 2D space. Works only in 2D scenes.
    void fillSquare2D(const Util::Math::Vector2<float> &position, float size) const;

    /// Draw a filled rectangle in 2D space. Works only in 2D scenes.
    void fillRectangle2D(const Util::Math::Vector2<float> &position, const Util::Math::Vector2<float> &size) const;

    /// Draw a string in 2D space. Works only in 2D scenes.
    void drawString2D(const Util::Math::Vector2<float> &position, const char *string) const;

    /// Draw a string in 2D space. Works only in 2D scenes.
    void drawString2D(const Util::Math::Vector2<float> &position, const Util::String &string) const;

    /// Draw an image in 2D space. Works only in 2D scenes.
    /// The image can be mirrored horizontally at no additional cost by setting `flipX` to true.
    /// The image can be drawn with transparency by setting `alpha` to a value between 0 (fully transparent)
    /// and 1 (fully opaque). It can also be scaled and rotated (in degrees) around its center.
    /// However, transparency, scaling and rotation are very expensive operations, with significant performance impact.
    void drawImage2D(const Util::Math::Vector2<float> &position, const Util::Graphic::Image &image, bool flipX = false,
        float alpha = 1, const Util::Math::Vector2<float> &scale = Util::Math::Vector2<float>(1, 1),
        float rotationAngle = 0) const;

    /***** 3D drawing functions *****/

    /// Draw a 3D model. Works only in 3D scenes.
    void drawModel3D(const D3::Model &model) const;

    /// Draw a colored cuboid in 3D space. Works only in 3D scenes.
    void drawCuboid3D(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &size,
        const Util::Math::Vector3<float> &rotation, const D3::Texture &texture = D3::Texture()) const;

    /// Draw a colored 2D rectangle in 3D space. Works only in 3D scenes.
    void drawRectangle3D(const Util::Math::Vector3<float> &position, const Util::Math::Vector2<float> &size,
        const Util::Math::Vector3<float> &rotation, const D3::Texture &texture = D3::Texture()) const;

    /// Draw a custom shape defined by vertices in 3D space. Works only in 3D scenes.
    void drawCustomShape3D(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &scale,
        const Util::Math::Vector3<float> &rotation, const Util::Array<Util::Math::Vector3<float>> &vertices) const;

    /// Draw a precompiled display list in 3D space.
    void drawList3D(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &scale,
        const Util::Math::Vector3<float> &rotation, GLuint list) const;

    /// Start compiling a new display list for 3D objects.
    /// After calling this function, all subsequent 3D drawing commands are recorded into the display list
    /// until `endList3D()` is called. The display list can then be drawn using `drawList3D()`.
    /// CAUTION: Only call 3D drawing functions starting with `list` (e.g., `listCuboid3D()`) between
    /// `startList3D()` and `endList3D()`. Calling other 3D drawing functions or any 2D drawing functions
    /// in between will lead to undefined behavior.
    static GLuint startList3D();

    /// End compiling the current display list for 3D objects.
    /// This function finalizes the current display list started with `startList3D()`.
    static void endList3D();

    /// List a 3D model for inclusion in a display list.
    static void listModel3D(const D3::Model &model);

    /// List a colored cuboid for inclusion in a display list.
    static void listCuboid3D(const Util::Math::Vector3<float> &size, const Util::Graphic::Color &color);

    /// List a colored cuboid for inclusion in a display list.
    static void listCuboid3D(const Util::Math::Vector3<float> &translation, const Util::Math::Vector3<float> &size,
        const Util::Graphic::Color &color);

    /// List a textured cuboid for inclusion in a display list.
    static void listCuboid3D(const Util::Math::Vector3<float> &size, const D3::Texture &texture = D3::Texture());

    /// List a textured cuboid for inclusion in a display list.
    static void listCuboid3D(const Util::Math::Vector3<float> &translation, const Util::Math::Vector3<float> &size,
        const D3::Texture &texture = D3::Texture());

    /// List a colored 2D rectangle for inclusion in a display list.
    static void listRectangle3D(const Util::Math::Vector2<float> &size, const Util::Graphic::Color &color);

    /// List a colored 2D rectangle for inclusion in a display list.
    static void listRectangle3D(const Util::Math::Vector2<float> &size, const D3::Texture &texture = D3::Texture());

    /// List a colored 2D rectangle for inclusion in a display list.
    static void listCustomShape3D(const Util::Array<Util::Math::Vector3<float>> &vertices);

    /***** Miscellaneous *****/

    /// Initialize OpenGL rendering.
    /// This function is called automatically when a 3D scene is loaded and should not be called manually.
    void initializeGl();

    /// Disable OpenGL rendering.
    /// This function is called automatically when a 2D scene is loaded and should not be called manually.
    void disableGl();

    /// Check if OpenGL rendering is enabled (i.e., if the current scene is a 3D scene).
    bool isGlEnabled() const;

    /// Clear the screen with the given color.
    /// For 2D scenes, using `Colors::BLACK` as the clear color is much faster than other colors.
    void clear(const Util::Graphic::Color &color = Util::Graphic::Colors::BLACK) const;

    /// Show the rendered frame on the screen.
    /// This function is called automatically by the engine after each frame and should not be called manually.
    void show() const;

    /// Update the graphics context.
    /// This function is called automatically by the engine after each frame and should not be called manually.
    void update();

    /// Set the current drawing color.
    /// All subsequent drawing operations will use this color until it is changed again.
    /// Images and textures are not affected by this color.
    void setColor(const Util::Graphic::Color &color);

    /// Get the current drawing color.
    const Util::Graphic::Color& getColor() const;

    /// Save the current frame buffer state as the background.
    /// This is used by 2D scenes to optimize redrawing static backgrounds.
    /// The background will scroll automatically with the camera, repeating itself over and over again.
    void saveCurrentStateAsBackground();

    /// Clear the saved background.
    void clearBackground();

    /// Get the camera associated with the current scene.
    const Camera& getCamera() const;

    /// Get the absolute horizontal screen resolution.
    uint16_t getAbsoluteResolutionX() const;

    /// Get the absolute vertical screen resolution.
    uint16_t getAbsoluteResolutionY() const;

    /// Get the dimensions of the game coordinate system.
    /// The game uses a virtual coordinate system ranging from (-1, -1) to (1, 1) for the entire screen.
    /// However, if the target screen resolution is not square,
    /// one axis will have a larger range to maintain the aspect ratio.
    /// For example, on a 800x600 screen, the coordinate system will range from (-1.33, -1) to (1.33, 1).
    const Util::Math::Vector2<float>& getDimensions() const;

    /// Get the screen transformation factor.
    /// This factor is used to scale from the game coordinate system to the actual screen resolution.
    /// The game uses a virtual coordinate system ranging from (-1, -1) to (1, 1) for the entire screen.
    /// The transformation factor can be multiplied to any coordinate in this system
    /// to get the corresponding pixel coordinate on the screen.
    /// The engine calculates the transformation factor by taking the smaller axis of the target screen resolution
    /// and dividing it by 2. For example, on a 800x600 screen, the transformation factor will be 300.
    uint16_t getTransformation() const;

    /// Get the font size in game coordinates.
    float getRelativeFontSize() const;

    /// The font size in absolute pixels.
    static constexpr uint8_t FONT_SIZE = 8;

private:

    friend class Engine;

    void drawImageDirect2D(const Util::Math::Vector2<float> &position, const Util::Graphic::Image &image,
        bool flipX, float alpha) const;

    void drawImageScaled2D(const Util::Math::Vector2<float> &position, const Util::Graphic::Image &image,
        bool flipX, float alpha, const Util::Math::Vector2<float> &scale) const;

    void drawImageRotated2D(const Util::Math::Vector2<float> &position, const Util::Graphic::Image &image,
        bool flipX, float alpha, float rotationAngle) const;

    void drawImageScaledAndRotated2D(const Util::Math::Vector2<float> &position, const Util::Graphic::Image &image,
        bool flipX, float alpha, const Util::Math::Vector2<float> &scale, float rotationAngle) const;

    void gluPrepareDirectDraw(GLint renderStyle) const;

    void gluFinishDirectDraw() const;

    /// Taken from https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
    static void gluPerspective(GLfloat fovY, GLfloat aspect, GLfloat zNear, GLfloat zFar);

    static void gluMultOrthoMatrix(float left, float right, float bottom, float top, float near, float far);

    const Util::Graphic::BufferedLinearFrameBuffer bufferedLfb;
    ZBuffer *glBuffer = nullptr;
    uint8_t *backgroundBuffer = nullptr;
    bool glEnabled = false;

    const uint16_t transformation;
    const uint16_t offsetX;
    const uint16_t offsetY;
    const Util::Math::Vector2<float> dimensions;

    Util::Graphic::Color color = Util::Graphic::Colors::WHITE;

    Camera camera;
};

}

#endif

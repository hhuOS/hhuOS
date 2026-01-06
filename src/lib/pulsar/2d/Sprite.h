/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_2D_SPRITE_H
#define HHUOS_LIB_PULSAR_2D_SPRITE_H

#include "util/graphic/Image.h"
#include "util/base/String.h"
#include "util/math/Vector2.h"
#include "pulsar/Graphics.h"

namespace Pulsar {
namespace D2 {

/// Represents a 2D sprite.
/// A sprite is a 2D image that can be drawn on the screen with various transformations
/// like scaling, rotation, and alpha blending. Usually, a bitmap image is used as the
/// source for the sprite, but it is also possible to create a rectangular sprite filled
/// with a single color (useful for debugging purposes or particles).
/// Loaded images are cached in the `Pulsar::Resources` class to avoid loading the same image multiple times.
/// Transformations like scaling and rotation are applied during rendering and do not modify
/// the original image data. They are very compute expensive and should be used sparingly on small
/// sprites only (e.g. particles).
class Sprite {

public:
    /// Create a new sprite instance with an empty image and size of (0, 0).
    Sprite();

    /// Create a new sprite instance from a bitmap image located at the given path.
    /// The image will be scaled to the specified width and height (in game units).
    Sprite(const Util::String &path, float width, float height);

    /// Create a new sprite instance filled with the given color.
    /// The sprite will have the specified width and height with the image being a filled rectangle of that color.
    Sprite(const Util::Graphic::Color &color, float width, float height);

    /// Get the underlying image of the sprite.
    const Util::Graphic::Image& getImage() const {
        return *image;
    }

    /// Get the original size of the sprite (before scaling).
    const Util::Math::Vector2<float>& getOriginalSize() const {
        return size;
    }

    /// Get the current size of the sprite (after scaling).
    Util::Math::Vector2<float> getSize() const {
        return Util::Math::Vector2<float>(size.getX() * scale.getX(), size.getY() * scale.getY());
    }

    /// Set the scale of the sprite uniformly in both dimensions.
    void setScale(const float scale) {
        setScale(Util::Math::Vector2<float>(scale, scale));
    }

    /// Set the scale of the sprite in both dimensions.
    void setScale(const Util::Math::Vector2<float> &scale) {
        Sprite::scale = scale;
    }

    /// Get the current scale of the sprite.
    const Util::Math::Vector2<float>& getScale() const {
        return scale;
    }

    /// Rotate the sprite by the given angle (in degrees).
    void rotate(const float angle) {
        rotationAngle += angle;
    }

    /// Set the rotation angle of the sprite (in degrees).
    void setRotation(const float angle) {
        rotationAngle = angle;
    }

    /// Get the current rotation angle of the sprite (in degrees).
    float getRotation() const {
        return rotationAngle;
    }

    /// Set the alpha transparency of the sprite (0.0 = fully transparent, 1.0 = fully opaque).
    void setAlpha(const float alpha) {
        Sprite::alpha = alpha;
    }

    /// Get the current alpha transparency of the sprite.
    float getAlpha() const {
        return alpha;
    }

    /// Flip the sprite horizontally.
    /// This causes the sprite to be mirrored along the vertical axis during rendering.
    /// If the sprite is already flipped, calling this method will un-flip it.
    void flipX() {
        xFlipped = !xFlipped;
    }

    /// Set whether the sprite is flipped horizontally (i.e. mirrored along the vertical axis).
    void setXFlipped(bool flipped) {
        xFlipped = flipped;
    }

    /// Draw the sprite at the given position using the specified graphics context.
    void draw(const Graphics &graphics, const Util::Math::Vector2<float> &position) const {
        graphics.drawImage2D(position, *image, xFlipped, alpha, scale, rotationAngle);
    }

private:

    const Util::Graphic::Image *image = nullptr;

    Util::Math::Vector2<float> size;
    Util::Math::Vector2<float> scale = Util::Math::Vector2<float>(1, 1);
    float rotationAngle = 0;
    float alpha = 1;
    bool xFlipped = false;
};

}
}

#endif

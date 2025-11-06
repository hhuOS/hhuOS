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
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_2D_SPRITEANIMATION_H
#define HHUOS_LIB_PULSAR_2D_SPRITEANIMATION_H

#include <stddef.h>

#include "util/collection/Array.h"
#include "util/math/Vector2.h"
#include "pulsar/2d/Sprite.h"

namespace Pulsar::D2 {

/// Represents a 2D sprite animation.
/// A sprite animation is a sequence of sprites that are displayed one after another
/// to create the illusion of motion. The animation calculates which sprite to display
/// based on the elapsed time and the total animation duration. It supports the same transformations
/// as the `Sprite` class, such as scaling, rotation, and alpha blending.
/// Transformations applied directly to a sprite before adding it to the animation will NOT be preserved.
/// The transformations must be applied to the `SpriteAnimation` instance itself.
class SpriteAnimation {

public:
    /// Create a new sprite animation instance with no sprites and zero animation time (empty animation).
    SpriteAnimation();

    /// Create a new sprite animation instance with the given sprites and total animation time.
    /// The animation time specifies how long it takes to play the entire animation once.
    SpriteAnimation(const Util::Array<Sprite> &sprites, double time);

    /// Reset the animation to the beginning (first sprite).
    void reset();

    /// Update the animation state based on the elapsed time since the last update.
    /// This method advances the animation according to the given delta time (in seconds).
    /// An entity using this animation should call this method every frame, during its own update cycle.
    void update(double delta);

    /// Get the total animation time (in seconds).
    [[nodiscard]] double getAnimationTime() const;

    /// Get the original size of the current sprite (before scaling).
    [[nodiscard]] const Util::Math::Vector2<double>& getOriginalSize() const;

    /// Get the current size of the current sprite (after scaling).
    [[nodiscard]] Util::Math::Vector2<double> getSize() const;

    /// Set the scale of the animation uniformly in both dimensions.
    void setScale(double scale);

    /// Set the scale of the animation in both dimensions.
    void setScale(const Util::Math::Vector2<double> &scale);

    /// Get the current scale of the animation.
    [[nodiscard]] const Util::Math::Vector2<double>& getScale() const;

    /// Rotate the animation by the given angle (in radians).
    void rotate(double angle);

    /// Set the rotation angle of the animation (in radians).
    void setRotation(double angle);

    /// Get the current rotation angle of the animation (in radians).
    [[nodiscard]] double getRotation() const;

    /// Set the alpha transparency of the animation (0.0 = fully transparent, 1.0 = fully opaque).
    void setAlpha(double alpha);

    /// Get the current alpha transparency of the animation.
    [[nodiscard]] double getAlpha() const;

    /// Flip the animation horizontally.
    /// This causes the animation to be mirrored along the vertical axis during rendering.
    /// If the animation is already flipped, calling this method will un-flip it.
    void flipX();

    /// Set whether the animation is flipped horizontally (i.e. mirrored along the vertical axis).
    void setXFlipped(bool flipped);

    /// Draw the current sprite of the animation at the given position using the specified graphics context.
    void draw(const Graphics &graphics, const Util::Math::Vector2<double> &position) const;

private:

    Util::Math::Vector2<double> scale = Util::Math::Vector2<double>(1, 1);
    double rotationAngle = 0;
    double alpha = 1;
    bool xFlipped = false;

    double animationTime = 0;
    double timePerSprite = 0;
    double timeSinceLastChange = 0;
    size_t currentSprite = 0;
    Util::Array<Sprite> sprites;
};

}

#endif

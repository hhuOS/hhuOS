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
 */

#ifndef HHUOS_APPLICATION_BUG_EXPLOSIVE_H
#define HHUOS_APPLICATION_BUG_EXPLOSIVE_H

#include <stddef.h>

#include <pulsar/audio/AudioHandle.h>
#include <pulsar/audio/AudioTrack.h>
#include <pulsar/2d/SpriteAnimation.h>
#include <pulsar/2d/Entity.h>

/// Super class for all game objects that can explode.
/// An explosive can play a short explosion animation with an appropriate sound effect.
/// Subclasses can call the function `Explosive::explode()`, when the entity (e.g., an enemy bug) has died.
/// Afterward, they must call `Explosive::onUpdate()` and `Explosive::draw()` in their own implementations
/// of `onUpdate()` and `draw()`.
class Explosive : public Pulsar::D2::Entity {

public:
    /// Create a new explosive object at a given position, playing the given sound file.
    Explosive(size_t tag, const Util::Math::Vector2<float> &position, const Pulsar::D2::RectangleCollider &collider,
        const Util::String &waveFilePath, float animationTime = 0.5);

    /// Initialize the explosive object, loading its animation sprites and sound file.
    void initialize() override;

    /// Update the animation according to the given time delta.
    void onUpdate(float delta) override;

    /// Draw the current animation sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Start the explosion animation and play the sound effect.
    void explode() {
        if (!exploding) {
            shouldExplode = true;
        }
    }

    /// Check whether the explosion animation is currently playing.
    /// This function also returns true if the animation has already finished.
    bool isExploding() const {
        return exploding;
    }

    /// Check whether the explosion animation has finished playing.
    bool hasExploded() const {
        return explosionTimer >= animation.getAnimationTime() && !soundEffectHandle.isPlaying();
    }

private:

    Util::String waveFilePath;
    Pulsar::AudioTrack soundEffect;
    Pulsar::AudioHandle soundEffectHandle;

    float animationTime;
    Pulsar::D2::SpriteAnimation animation;

    bool shouldExplode = false;
    bool exploding = false;

    float explosionTimer = 0;
};

#endif

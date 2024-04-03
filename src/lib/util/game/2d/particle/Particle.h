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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_PARTICLE_H
#define HHUOS_PARTICLE_H

#include <cstdint>

#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/Sprite.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

namespace Util::Game::D2 {

class Emitter;

class Particle : public Entity {

public:
    /**
     * Constructor.
     */
    Particle(uint32_t tag, Emitter &parent);

    /**
     * Copy Constructor.
     */
    Particle(const Particle &other) = delete;

    /**
     * Assignment operator.
     */
    Particle &operator=(const Particle &other) = delete;

    /**
     * Destructor.
     */
    ~Particle() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Graphics &graphics) override;

    void onTranslationEvent(TranslationEvent &event) override;

    void onCollisionEvent(CollisionEvent &event) override;

    [[nodiscard]] bool isParticle() const override;

    [[nodiscard]] double getScale() const;

    void setScale(double scale);

    [[nodiscard]] double getAlpha() const;

    void setAlpha(double alpha);

    void setTimeToLive(double timeToLive);

    void setSprite(const Sprite &sprite);

    void setRotationVelocity(double rotationVelocity);

    [[nodiscard]] double getRotationVelocity() const;

private:

    double rotationVelocity = 0;
    bool timeLimited = false;
    double timeToLive = -1;

    Sprite sprite;
    Emitter &parent;
};

}

#endif

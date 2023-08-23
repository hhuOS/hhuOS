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
 */

#ifndef HHUOS_MOUSECURSOR_H
#define HHUOS_MOUSECURSOR_H

#include <cstdint>

#include "lib/util/game/MouseListener.h"
#include "lib/util/game/entity/Entity.h"
#include "lib/util/game/Sprite.h"

class Logo;

namespace Util {
namespace Io {
namespace Mouse {
enum Button : uint8_t;
enum ScrollDirection : uint8_t;
}  // namespace Mouse
}  // namespace Io

namespace Math {
class Vector2D;
}  // namespace Math

namespace Game {
class Graphics;
class CollisionEvent;
class TranslationEvent;
}  // namespace Game
}  // namespace Util

class MouseCursor : public Util::Game::Entity, public Util::Game::MouseListener {

public:
    /**
     * Default Constructor.
     */
    explicit MouseCursor(Logo &logo);

    /**
     * Copy Constructor.
     */
    MouseCursor(const MouseCursor &other) = delete;

    /**
     * Assignment operator.
     */
    MouseCursor &operator=(const MouseCursor &other) = delete;

    /**
     * Destructor.
     */
    ~MouseCursor() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Game::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::CollisionEvent &event) override;

    void draw(Util::Game::Graphics &graphics) override;

    void buttonPressed(Util::Io::Mouse::Button key) override;

    void buttonReleased(Util::Io::Mouse::Button key) override;

    void mouseMoved(const Util::Math::Vector2D &relativeMovement) override;

    void mouseScrolled(Util::Io::Mouse::ScrollDirection direction) override;

private:

    Util::Game::Sprite defaultSprite = Util::Game::Sprite("/initrd/mouse/mouse_default.bmp", 0.2, 0.2);
    Util::Game::Sprite leftClickSprite = Util::Game::Sprite("/initrd/mouse/mouse_left_click.bmp", 0.2, 0.2);
    Util::Game::Sprite rightClickSprite = Util::Game::Sprite("/initrd/mouse/mouse_right_click.bmp", 0.2, 0.2);
    Util::Game::Sprite middleClickSprite = Util::Game::Sprite("/initrd/mouse/mouse_scroll.bmp", 0.2, 0.2);

    bool button4Pressed = false;
    bool button5Pressed = false;

    Util::Game::Sprite *currentSprite = &defaultSprite;
    Logo &logo;
};

#endif

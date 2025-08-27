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
 */

#ifndef HHUOS_MOUSECURSOR_H
#define HHUOS_MOUSECURSOR_H

#include "lib/util/game/MouseListener.h"
#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/base/String.h"

class Logo;

class MouseCursor : public Util::Game::D2::Entity, public Util::Game::MouseListener {

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

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void draw(Util::Game::Graphics &graphics) override;

    void buttonPressed(Util::Io::MouseDecoder::Button button) override;

    void buttonReleased(Util::Io::MouseDecoder::Button button) override;

    void mouseMoved(const Util::Math::Vector2<double> &relativeMovement) override;

    void mouseScrolled(Util::Io::MouseDecoder::ScrollDirection direction) override;

private:

    Util::Game::D2::Sprite defaultSprite = Util::Game::D2::Sprite("/user/mouse/mouse_default.bmp", 0.2, 0.2);
    Util::Game::D2::Sprite leftClickSprite = Util::Game::D2::Sprite("/user/mouse/mouse_left_click.bmp", 0.2, 0.2);
    Util::Game::D2::Sprite rightClickSprite = Util::Game::D2::Sprite("/user/mouse/mouse_right_click.bmp", 0.2, 0.2);
    Util::Game::D2::Sprite middleClickSprite = Util::Game::D2::Sprite("/user/mouse/mouse_scroll.bmp", 0.2, 0.2);

    bool button4Pressed = false;
    bool button5Pressed = false;

    Util::Game::D2::Sprite *currentSprite = &defaultSprite;
    Logo &logo;
};

#endif

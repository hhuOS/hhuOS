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
 */

#include "MouseCursor.h"

#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/game/GameManager.h"
#include "Logo.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/graphic/Font.h"

namespace Util {
namespace Game {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

MouseCursor::MouseCursor(Logo &logo) : Util::Game::D2::Entity(0, Util::Math::Vector2D(0, 0)), logo(logo) {}

void MouseCursor::initialize() {}

void MouseCursor::onUpdate([[maybe_unused]] double delta) {}

void MouseCursor::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    const auto &resolution = Util::Game::GameManager::getRelativeResolution();
    const auto &newPosition = event.getTargetPosition();

    if (newPosition.getX() < -resolution.getX() ||
        newPosition.getX() > resolution.getX() - currentSprite->getSize().getX() ||
        newPosition.getY() < -resolution.getY() ||
        newPosition.getY() > resolution.getY() - currentSprite->getSize().getY()) {
        event.cancel();
    }
}

void MouseCursor::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void MouseCursor::draw(Util::Game::Graphics &graphics) {
    auto &font = Util::Graphic::Font::getFontForResolution(static_cast<uint32_t>(Util::Game::GameManager::getAbsoluteResolution().getY()));
    auto charWidth = font.getCharWidth() / static_cast<double>(Util::Game::GameManager::getTransformation());
    auto additionalButtons = Util::String::format("%c%c", button4Pressed ? '4' : ' ', button5Pressed ? '5' : ' ');

    currentSprite->draw(graphics, getPosition());
    graphics.setColor(Util::Graphic::Colors::HHU_TURQUOISE);
    graphics.drawString2D(font, getPosition() + Util::Math::Vector2D(currentSprite->getSize().getX() / 2 - charWidth, currentSprite->getSize().getY() / 3), additionalButtons);
}

void MouseCursor::buttonPressed(Util::Io::Mouse::Button button) {
    switch (button) {
        case Util::Io::Mouse::LEFT_BUTTON:
            currentSprite = &leftClickSprite;
            break;
        case Util::Io::Mouse::RIGHT_BUTTON:
            currentSprite = &rightClickSprite;
            break;
        case Util::Io::Mouse::MIDDLE_BUTTON:
            currentSprite = &middleClickSprite;
            break;
        case Util::Io::Mouse::BUTTON_4:
            button4Pressed = true;
            break;
        case Util::Io::Mouse::BUTTON_5:
            button5Pressed = true;
            break;
        default:
            break;
    }
}

void MouseCursor::buttonReleased(Util::Io::Mouse::Button button) {
    switch (button) {
        case Util::Io::Mouse::BUTTON_4:
            button4Pressed = false;
            break;
        case Util::Io::Mouse::BUTTON_5:
            button5Pressed = false;
            break;
        default:
            currentSprite = &defaultSprite;
            break;
    }
}

void MouseCursor::mouseMoved(const Util::Math::Vector2D &relativeMovement) {
    translate(relativeMovement);
}

void MouseCursor::mouseScrolled(Util::Io::Mouse::ScrollDirection direction) {
    switch (direction) {
        case Util::Io::Mouse::UP:
            logo.translateY(0.01);
            break;
        case Util::Io::Mouse::DOWN:
            logo.translateY(-0.01);
            break;
        case Util::Io::Mouse::RIGHT:
            logo.translateX(0.01);
            break;
        case Util::Io::Mouse::LEFT:
            logo.translateX(-0.01);
            break;
        default:
            break;
    }
}

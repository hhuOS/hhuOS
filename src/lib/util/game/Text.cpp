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

#include "Text.h"

namespace Util::Game {

Text::Text(const Math::Vector2D &position, const String &text, const Graphic::Color &color) : Entity(0, position), text(text), color(color) {}

void Text::draw(Graphics2D &graphics) {
    graphics.setColor(color);
    graphics.drawString(getPosition(), text);
}

void Text::onUpdate(double delta) {}

void Text::onTranslationEvent(TranslationEvent &event) {}

void Text::onCollisionEvent(CollisionEvent &event) {}

const Graphic::Color &Text::getColor() const {
    return color;
}

void Text::setColor(const Graphic::Color &color) {
    Text::color = color;
}

const String &Text::getText() const {
    return text;
}

void Text::setText(const String &text) {
    Text::text = text;
}

}
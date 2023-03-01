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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#ifndef HHUOS_TEXT_H
#define HHUOS_TEXT_H

#include "lib/util/base/String.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/game/entity/Entity.h"
#include "lib/util/graphic/Colors.h"

namespace Util {
namespace Game {
class CollisionEvent;
class Graphics2D;
class TranslationEvent;
}  // namespace Game
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

namespace Util::Game {

class Text : public Entity {

public:
    /**
     * Constructor.
     */
    Text(const Math::Vector2D &position, const String &text, const Graphic::Color &color = Graphic::Colors::WHITE);

    /**
     * Copy Constructor.
     */
    Text(const Text &other) = delete;

    /**
     * Assignment operator.
     */
    Text &operator=(const Text &other) = delete;

    /**
     * Destructor.
     */
    ~Text() override = default;

    void initialize() override;
    
    void draw(Graphics2D &graphics) override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Game::TranslationEvent &event) final;

    void onCollisionEvent(Util::Game::CollisionEvent &event) final;

    [[nodiscard]] const Graphic::Color &getColor() const;

    void setColor(const Graphic::Color &color);

    [[nodiscard]] const String &getText() const;

    void setText(const String &text);

private:

    String text;
    Graphic::Color color;
};

}

#endif

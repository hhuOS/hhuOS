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
 */

#ifndef HHUOS_SPRITE_H
#define HHUOS_SPRITE_H

#include "util/graphic/Image.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector2.h"

namespace Pulsar {
class Graphics;
}  // namespace Util

namespace Pulsar::D2 {

class Sprite {

public:
    /**
     * Default Constructor.
     */
    Sprite();

    /**
     * Constructor.
     */
    Sprite(const Util::String &path, double width, double height);

    /**
     * Copy Constructor.
     */
    Sprite(const Sprite &other) = default;

    /**
     * Assignment operator.
     */
    Sprite &operator=(const Sprite &other) = default;

    /**
     * Destructor.
     */
    ~Sprite() = default;

    [[nodiscard]] const Util::Graphic::Image& getImage() const;

    [[nodiscard]] const Util::Math::Vector2<double>& getOriginalSize() const;

    [[nodiscard]] Util::Math::Vector2<double> getSize() const;

    [[nodiscard]] const Util::Math::Vector2<double>& getScale() const;

    [[nodiscard]] double getRotation() const;

    [[nodiscard]] double getAlpha() const;

    void setScale(const Util::Math::Vector2<double> &scale);

    void setScale(double scale);

    void setRotation(double angle);

    void setAlpha(double alpha);

    void rotate(double angle);

    void flipX();

    void draw(const Graphics &graphics, const Util::Math::Vector2<double> &position) const;

private:

    const Util::Graphic::Image *image;

    Util::Math::Vector2<double> size;
    Util::Math::Vector2<double> scale = Util::Math::Vector2<double>(1, 1);
    double rotationAngle = 0;
    double alpha = 1;
    bool xFlipped = false;
};

}

#endif

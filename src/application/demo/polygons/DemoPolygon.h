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

#ifndef HHUOS_DEMOPOLYGON_H
#define HHUOS_DEMOPOLYGON_H

#include "lib/pulsar/2d/Polygon.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"
#include "lib/pulsar/2d/Entity.h"
#include "lib/util/math/Vector2.h"

class DemoPolygon : public Pulsar::D2::Entity {

public:
    /**
     * Default Constructor.
     */
    DemoPolygon();

    /**
     * Constructor.
     */
    DemoPolygon(const Util::Array<Util::Math::Vector2<double>> &vertices, const Util::Math::Vector2<double> &position, const Util::Graphic::Color &color, double initialScaleFactor, double scaleSpeed, double rotationSpeed);

    /**
     * Copy Constructor.
     */
    DemoPolygon(const DemoPolygon &other) = delete;

    /**
     * Assignment operator.
     */
    DemoPolygon &operator=(const DemoPolygon &other) = delete;

    /**
     * Destructor.
     */
    ~DemoPolygon() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(Pulsar::D2::CollisionEvent &event) override;

    void draw(Pulsar::Graphics &graphics) const override;

private:

    Pulsar::D2::Polygon polygon;
    Util::Graphic::Color color;
    double rotationSpeed;
    double scaleSpeed;
    double currentScale = 1;
    bool scaleUp = true;
};

#endif
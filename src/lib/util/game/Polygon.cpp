/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Polygon.h"
#include "lib/util/math/Math.h"

namespace Util::Game {

Polygon::Polygon(const Data::Array<double> &x, const Data::Array<double> &y) : x(x), y(y) {
    if (x.length() != y.length()) {
        Exception::throwException(Exception::INVALID_ARGUMENT, "Polygon: x and y arrays must have the same size!");
    }

    calculateCenter();
}

void Polygon::draw(Graphics2D &graphics) const {
    graphics.drawPolygon(x, y);
}

void Polygon::scale(double factor) {
    for (uint32_t i = 0; i < x.length(); i++) {
        x[i] = xCenter + factor * (x[i] - xCenter);
        y[i] = yCenter + factor * (y[i] - yCenter);
    }
}

void Polygon::rotate(double angle) {
    double sine = Math::Math::sine(angle);
    double cosine = Math::Math::cosine(angle);

    for (uint32_t i = 0; i < x.length(); i++) {
        double dx = x[i] - xCenter;
        double dy = y[i] - yCenter;
        x[i] = dx * cosine - dy * sine + xCenter;
        y[i] = dx * sine + dy * cosine + yCenter;
    }
}

void Polygon::translate(double xTranslation, double yTranslation) {
    for (uint32_t i = 0; i < x.length(); i++) {
        x[i] += xTranslation;
        y[i] += yTranslation;
    }

    xCenter += xTranslation;
    yCenter += yTranslation;
}

void Polygon::calculateCenter() {
    double xSum = 0;
    double ySum = 0;

    for (uint32_t i = 0; i < x.length(); i++) {
        xSum += x[i];
        ySum += y[i];
    }

    xCenter = xSum / x.length();
    yCenter = ySum / y.length();
}

}
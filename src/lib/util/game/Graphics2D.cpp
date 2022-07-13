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

#include "lib/util/graphic/Fonts.h"
#include "Graphics2D.h"

namespace Util::Game {

Graphics2D::Graphics2D(const Graphic::LinearFrameBuffer &lfb) :
    lfb(lfb), pixelDrawer(Graphics2D::lfb), lineDrawer(pixelDrawer), stringDrawer(pixelDrawer),
    transformation((lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2),
    offsetX(transformation + (lfb.getResolutionX() > lfb.getResolutionY() ? (lfb.getResolutionX() - lfb.getResolutionY()) / 2 : 0)),
    offsetY(transformation + (lfb.getResolutionY() > lfb.getResolutionX() ? (lfb.getResolutionY() - lfb.getResolutionX()) / 2 : 0)) {}

void Graphics2D::drawLine(double x1, double y1, double x2, double y2) const {
    lineDrawer.drawLine(static_cast<int32_t>(x1 * transformation + offsetX), static_cast<int32_t>(-y1 * transformation + offsetY),
                        static_cast<int32_t>(x2 * transformation + offsetX), static_cast<int32_t>(-y2 * transformation + offsetY), color);
}

void Graphics2D::drawPolygon(const Data::Array<double> &x, const Data::Array<double> &y) const {
    for (uint32_t i = 0; i < x.length() - 1; i++) {
        drawLine(x[i], y[i], x[i + 1], y[i + 1]);
    }

    drawLine(x[x.length() - 1], y[y.length() - 1], x[0], y[0]);
}

void Graphics2D::drawString(const Graphic::Font &font, double x, double y, const char *string) const {
    stringDrawer.drawString(font, static_cast<int32_t>(x * transformation + offsetX), static_cast<int32_t>(-y * transformation + offsetY), string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics2D::drawString(double x, double y, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT, x, y, string);
}

void Graphics2D::drawString(double x, double y, const Util::Memory::String &string) const {
    drawString(x, y, static_cast<const char *>(string));
}

void Graphics2D::drawStringSmall(double x, double y, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT_SMALL, x, y, string);
}

void Graphics2D::drawStringSmall(double x, double y, const Util::Memory::String &string) const {
    drawStringSmall(x, y, static_cast<const char *>(string));
}

void Graphics2D::show() const {
    lfb.flush();
    lfb.clear();
}

void Graphics2D::setColor(const Graphic::Color &color) {
    Graphics2D::color = color;
}

Graphic::Color Graphics2D::getColor() const {
    return color;
}

}
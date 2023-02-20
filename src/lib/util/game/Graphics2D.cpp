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
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/math/Vector2D.h"
#include "Game.h"
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Image.h"

namespace Util {
namespace Graphic {
class Font;
}  // namespace Graphic
}  // namespace Util

namespace Util::Game {

Graphics2D::Graphics2D(const Graphic::LinearFrameBuffer &lfb, Game &game) :
    game(game), lfb(lfb), pixelDrawer(Graphics2D::lfb), lineDrawer(pixelDrawer), stringDrawer(pixelDrawer),
    transformation((lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2),
    offsetX(transformation + (lfb.getResolutionX() > lfb.getResolutionY() ? (lfb.getResolutionX() - lfb.getResolutionY()) / 2 : 0)),
    offsetY(transformation + (lfb.getResolutionY() > lfb.getResolutionX() ? (lfb.getResolutionY() - lfb.getResolutionX()) / 2 : 0)) {}

void Graphics2D::drawLine(const Math::Vector2D &from, const Math::Vector2D &to) const {
    auto &camera = game.getCamera();
    lineDrawer.drawLine(static_cast<int32_t>((from.getX() - camera.getPosition().getX()) * transformation + offsetX),
                        static_cast<int32_t>((-from.getY() + camera.getPosition().getY()) * transformation + offsetY),
                        static_cast<int32_t>((to.getX() - camera.getPosition().getX()) * transformation + offsetX),
                        static_cast<int32_t>((-to.getY() + camera.getPosition().getY()) * transformation + offsetY), color);
}

void Graphics2D::drawPolygon(const Array<Math::Vector2D> &vertices) const {
    for (uint32_t i = 0; i < vertices.length() - 1; i++) {
        drawLine(vertices[i], vertices[i + 1]);
    }

    drawLine(vertices[vertices.length() - 1], vertices[0]);
}

void Graphics2D::drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    stringDrawer.drawString(font, static_cast<int32_t>(position.getX() * transformation + offsetX), static_cast<int32_t>(-position.getY() * transformation + offsetY), string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics2D::drawString(const Math::Vector2D &position, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT, position, string);
}

void Graphics2D::drawString(const Math::Vector2D &position, const String &string) const {
    drawString(position, static_cast<const char*>(string));
}

void Graphics2D::drawStringSmall(const Math::Vector2D &position, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT_SMALL, position, string);
}

void Graphics2D::drawStringSmall(const Math::Vector2D &position, const String &string) const {
    drawStringSmall(position, static_cast<const char *>(string));
}

void Graphics2D::drawImage(const Math::Vector2D &position, const Graphic::Image &image, bool flipX) const {
    auto &camera = game.getCamera();
    auto pixelBuffer = image.getPixelBuffer();
    auto xFlipOffset = flipX ? image.getWidth() - 1 : 0;
    auto xPixelOffset = static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    auto yPixelOffset = static_cast<int32_t>((-position.getY() + camera.getPosition().getY()) * transformation + offsetY);

    if (xPixelOffset + image.getWidth() < 0 || xPixelOffset > pixelDrawer.getWidth() ||
        yPixelOffset - image.getHeight() > pixelDrawer.getHeight() || yPixelOffset < 0) {
        return;
    }

    for (int32_t i = 0; i < image.getHeight(); i++) {
        for (int32_t j = 0; j < image.getWidth(); j++) {
            pixelDrawer.drawPixel(xPixelOffset + xFlipOffset + (flipX ? -1 : 1) * j, yPixelOffset - i, pixelBuffer[i * image.getWidth() + j]);
        }
    }
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
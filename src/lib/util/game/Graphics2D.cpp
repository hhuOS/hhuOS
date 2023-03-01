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

#include "lib/util/graphic/Fonts.h"
#include "Graphics2D.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/math/Vector2D.h"
#include "Game.h"
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Scene.h"

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
    auto &camera = game.getCurrentScene().getCamera().getPosition();
    lineDrawer.drawLine(static_cast<int32_t>((from.getX() - camera.getX()) * transformation + offsetX),
                        static_cast<int32_t>((-from.getY() + camera.getY()) * transformation + offsetY),
                        static_cast<int32_t>((to.getX() - camera.getX()) * transformation + offsetX),
                        static_cast<int32_t>((-to.getY() + camera.getY()) * transformation + offsetY), color);
}

void Graphics2D::drawPolygon(const Array<Math::Vector2D> &vertices) const {
    for (uint32_t i = 0; i < vertices.length() - 1; i++) {
        drawLine(vertices[i], vertices[i + 1]);
    }

    drawLine(vertices[vertices.length() - 1], vertices[0]);
}

void Graphics2D::drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    auto &camera = game.getCurrentScene().getCamera().getPosition();
    stringDrawer.drawString(font, static_cast<int32_t>((position.getX() - camera.getX()) * transformation + offsetX), static_cast<int32_t>((-position.getY() + camera.getY()) * transformation + offsetY), string, color, Util::Graphic::Colors::INVISIBLE);
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
    drawStringSmall(position, static_cast<const char*>(string));
}

void Graphics2D::drawImage(const Math::Vector2D &position, const Graphic::Image &image, bool flipX) const {
    auto &camera = game.getCurrentScene().getCamera().getPosition();
    auto pixelBuffer = image.getPixelBuffer();
    auto xFlipOffset = flipX ? image.getWidth() - 1 : 0;
    auto xPixelOffset = static_cast<int32_t>((position.getX() - camera.getX()) * transformation + offsetX);
    auto yPixelOffset = static_cast<int32_t>((-position.getY() + camera.getY()) * transformation + offsetY);

    if (xPixelOffset + image.getWidth() < 0 || xPixelOffset > lfb.getResolutionX() ||
        yPixelOffset - image.getHeight() > lfb.getResolutionY() || yPixelOffset < 0) {
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

    if (backgroundBuffer == nullptr) {
        lfb.clear();
    } else {
        auto pitch = lfb.getPitch();
        auto colorDepthDivisor = (lfb.getColorDepth() == 15 ? 16 : lfb.getColorDepth()) / 8;
        auto xOffset = static_cast<uint32_t>(game.getCurrentScene().getCamera().getPosition().getX() * pitch / 4) % pitch;
        xOffset -= xOffset % colorDepthDivisor;

        for (uint32_t i = 0; i < lfb.getResolutionY(); i++) {
            auto yOffset = pitch * i;

            auto source = Address<uint32_t>(backgroundBuffer + yOffset + xOffset);
            auto target = lfb.getBuffer().add(yOffset);
            target.copyRange(source, pitch - xOffset);

            source = Address<uint32_t>(backgroundBuffer + yOffset);
            target = lfb.getBuffer().add(yOffset + (pitch - xOffset));
            target.copyRange(source, pitch - (pitch - xOffset));
        }
    }
}

void Graphics2D::setColor(const Graphic::Color &color) {
    Graphics2D::color = color;
}

Graphic::Color Graphics2D::getColor() const {
    return color;
}

void Graphics2D::saveCurrentStateAsBackground() {
    if (backgroundBuffer == nullptr) {
        backgroundBuffer = new uint8_t[lfb.getPitch() * lfb.getResolutionY()];
    }

    Address<uint32_t>(backgroundBuffer).copyRange(lfb.getBuffer(), lfb.getPitch() * lfb.getResolutionY());
}

void Graphics2D::clear(const Graphic::Color &color) {
    if (color == Util::Graphic::Colors::BLACK) {
        lfb.clear();
    } else {
        for (uint32_t i = 0; i < lfb.getResolutionX(); i++) {
            for (uint32_t j = 0; j < lfb.getResolutionY(); j++) {
                pixelDrawer.drawPixel(i, j, color);
            }
        }
    }
}

Math::Vector2D Graphics2D::getAbsoluteResolution() const {
    return Math::Vector2D(lfb.getResolutionX(), lfb.getResolutionY());
}

void Graphics2D::drawSquare(const Math::Vector2D &position, double size) const {
    drawRectangle(position, size, size);
}

void Graphics2D::drawRectangle(const Math::Vector2D &position, double width, double height) const {
    auto x = position.getX();
    auto y = position.getY();

    drawLine(position, Math::Vector2D(x + width, y));
    drawLine(Math::Vector2D(x, y - height), Math::Vector2D(x + width, y - height));
    drawLine(position, Math::Vector2D(x, y - height));
    drawLine(Math::Vector2D(x + width, y), Math::Vector2D(x + width, y - height));
}

void Graphics2D::fillSquare(const Math::Vector2D &position, double size) const {
    fillRectangle(position, size, size);
}

void Graphics2D::fillRectangle(const Math::Vector2D &position, double width, double height) const {
    auto &camera = game.getCurrentScene().getCamera().getPosition();
    auto startX = static_cast<int32_t>((position.getX() - camera.getX()) * transformation + offsetX);
    auto endX = static_cast<int32_t>((position.getX() + width - camera.getX()) * transformation + offsetX);
    auto startY = static_cast<int32_t>((-position.getY() + camera.getY()) * transformation + offsetY);
    auto endY = static_cast<int32_t>((-position.getY() + height + camera.getY()) * transformation + offsetY);

    for (int32_t i = startY; i < endY; i++) {
        lineDrawer.drawLine(startX, i, endX, i, color);
    }
}

}
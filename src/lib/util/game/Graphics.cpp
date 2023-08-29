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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "lib/util/graphic/Fonts.h"
#include "Graphics.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/math/Vector2D.h"
#include "Game.h"
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Scene.h"
#include "lib/util/math/Math.h"

namespace Util {
namespace Graphic {
class Font;
}  // namespace Graphic
}  // namespace Util

namespace Util::Game {

Graphics::Graphics(const Graphic::LinearFrameBuffer &lfb, Game &game) :
        game(game), lfb(lfb), pixelDrawer(Graphics::lfb), lineDrawer(pixelDrawer), stringDrawer(pixelDrawer),
        transformation((lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2),
        offsetX(transformation + (lfb.getResolutionX() > lfb.getResolutionY() ? (lfb.getResolutionX() - lfb.getResolutionY()) / 2 : 0)),
        offsetY(transformation + (lfb.getResolutionY() > lfb.getResolutionX() ? (lfb.getResolutionY() - lfb.getResolutionX()) / 2 : 0)) {}

/***** Basic functions to draw directly on the screen ******/

void Graphics::drawLine(const Math::Vector2D &from, const Math::Vector2D &to) const {
    lineDrawer.drawLine(static_cast<int32_t>(from.getX()), static_cast<int32_t>(from.getY()),
                        static_cast<int32_t>(to.getX()), static_cast<int32_t>(to.getY()), color);

}

void Graphics::drawPolygon(const Array<Math::Vector2D> &vertices) const {
    for (uint32_t i = 0; i < vertices.length() - 1; i++) {
        drawLine(vertices[i], vertices[i + 1]);
    }

    drawLine(vertices[vertices.length() - 1], vertices[0]);
}

void Graphics::drawRectangle(const Math::Vector2D &position, double width, double height) const {
    const auto x = position.getX();
    const auto y = position.getY();

    drawLine(position, Math::Vector2D(x + width, y));
    drawLine(Math::Vector2D(x, y + height), Math::Vector2D(x + width, y + height));
    drawLine(position, Math::Vector2D(x, y + height));
    drawLine(Math::Vector2D(x + width, y), Math::Vector2D(x + width, y + height));

}

void Graphics::drawSquare(const Math::Vector2D &position, double size) const {
    drawRectangle(position, size, size);
}

void Graphics::fillRectangle(const Math::Vector2D &position, double width, double height) const {
    const auto startX = static_cast<int32_t>(position.getX());
    const auto endX = static_cast<int32_t>(position.getX() + width);
    const auto startY = static_cast<int32_t>(position.getY());
    const auto endY = static_cast<int32_t>(position.getY() + height);

    for (int32_t i = startY; i < endY; i++) {
        lineDrawer.drawLine(startX, i, endX, i, color);
    }

}

void Graphics::fillSquare(const Math::Vector2D &position, double size) const {
    fillRectangle(position, size, size);
}

void Graphics::drawString(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    stringDrawer.drawString(font, static_cast<uint16_t>(position.getX()), static_cast<uint16_t>(position.getY()), string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics::drawString(const Math::Vector2D &position, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT, position, string);
}

void Graphics::drawString(const Math::Vector2D &position, const String &string) const {
    drawString(position, static_cast<const char*>(string));
}

void Graphics::drawStringSmall(const Math::Vector2D &position, const char *string) const {
    drawString(Graphic::Fonts::TERMINAL_FONT_SMALL, position, string);
}

void Graphics::drawStringSmall(const Math::Vector2D &position, const String &string) const {
    drawStringSmall(position, static_cast<const char*>(string));
}

/***** 2D drawing functions, respecting the camera position *****/

void Graphics::drawLine2D(const Math::Vector2D &from, const Math::Vector2D &to) const {
    lineDrawer.drawLine(static_cast<int32_t>((from.getX() - cameraPosition.getX()) * transformation + offsetX),
                        static_cast<int32_t>((-from.getY() + cameraPosition.getY()) * transformation + offsetY),
                        static_cast<int32_t>((to.getX() - cameraPosition.getX()) * transformation + offsetX),
                        static_cast<int32_t>((-to.getY() + cameraPosition.getY()) * transformation + offsetY), color);
}

void Graphics::drawPolygon2D(const Array<Math::Vector2D> &vertices) const {
    for (uint32_t i = 0; i < vertices.length() - 1; i++) {
        drawLine2D(vertices[i], vertices[i + 1]);
    }

    drawLine2D(vertices[vertices.length() - 1], vertices[0]);
}

void Graphics::drawSquare2D(const Math::Vector2D &position, double size) const {
    drawRectangle2D(position, size, size);
}

void Graphics::drawRectangle2D(const Math::Vector2D &position, double width, double height) const {
    auto x = position.getX();
    auto y = position.getY();

    drawLine2D(position, Math::Vector2D(x + width, y));
    drawLine2D(Math::Vector2D(x, y - height), Math::Vector2D(x + width, y - height));
    drawLine2D(position, Math::Vector2D(x, y - height));
    drawLine2D(Math::Vector2D(x + width, y), Math::Vector2D(x + width, y - height));
}

void Graphics::fillSquare2D(const Math::Vector2D &position, double size) const {
    fillRectangle2D(position, size, size);
}

void Graphics::fillRectangle2D(const Math::Vector2D &position, double width, double height) const {
    auto startX = static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    auto endX = static_cast<int32_t>((position.getX() + width - cameraPosition.getX()) * transformation + offsetX);
    auto startY = static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);
    auto endY = static_cast<int32_t>((-position.getY() + height + cameraPosition.getY()) * transformation + offsetY);

    for (int32_t i = startY; i < endY; i++) {
        lineDrawer.drawLine(startX, i, endX, i, color);
    }
}

void Graphics::drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    stringDrawer.drawString(font, static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX), static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY), string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics::drawString2D(const Math::Vector2D &position, const char *string) const {
    drawString2D(Graphic::Fonts::TERMINAL_FONT, position, string);
}

void Graphics::drawString2D(const Math::Vector2D &position, const String &string) const {
    drawString2D(position, static_cast<const char*>(string));
}

void Graphics::drawStringSmall2D(const Math::Vector2D &position, const char *string) const {
    drawString2D(Graphic::Fonts::TERMINAL_FONT_SMALL, position, string);
}

void Graphics::drawStringSmall2D(const Math::Vector2D &position, const String &string) const {
    drawStringSmall2D(position, static_cast<const char*>(string));
}

void Graphics::drawImage2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX) const {
    auto pixelBuffer = image.getPixelBuffer();
    auto xFlipOffset = flipX ? image.getWidth() - 1 : 0;
    auto xPixelOffset = static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    auto yPixelOffset = static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);

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

// Based on https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
Math::Vector2D Graphics::projectPoint(const Math::Vector3D &vertex, const Math::Vector3D &cameraPosition, const Math::Vector3D &cameraRotation) {
    const auto unitVector = Math::Vector3D(0, 0, 1);
    const auto lineDirection = unitVector.rotate(cameraRotation);
    const auto distanceToClosestPointOnLine = (vertex - cameraPosition) * lineDirection;

    if (distanceToClosestPointOnLine <= 0) {
        return Util::Math::Vector2D(-2, -2);
    }

    // Convert degree to radians
    const auto cameraRotationRadians = cameraRotation * (Math::PI / 180);

    const double x = cameraRotationRadians.getX();
    const double y = cameraRotationRadians.getY();
    const double z = cameraRotationRadians.getZ();

    const double sinX = Util::Math::sine(x);
    const double cosX = Util::Math::cosine(x);
    const double sinY = Util::Math::sine(y);
    const double cosY = Util::Math::cosine(y);
    const double sinZ = Util::Math::sine(z);
    const double cosZ = Util::Math::cosine(z);

    const auto rotationMatrix = Math::Matrix3x3 {
            cosY * cosZ, cosY * sinZ, -sinY,
            sinX * sinY * cosZ - cosX * sinZ, sinX * sinY * sinZ + cosX * cosZ, sinX * cosY,
            cosX * sinY * cosZ + sinX * sinZ, cosX * sinY * sinZ - sinX * cosZ, cosX * cosY
    };

    const auto d = rotationMatrix * (vertex - cameraPosition);
    const auto e = Util::Math::Vector3D(0, 0, FIELD_OF_VIEW);
    const auto a = d.getZ() == 0 ? 1 : e.getZ() / d.getZ();

    return {
            a * d.getX() + e.getX(),
            a * d.getY() + e.getY()
    };
}

void Graphics::drawLine3D(const Math::Vector3D &from, const Math::Vector3D &to) {
    const auto v1 = projectPoint(from, cameraPosition, cameraRotation);
    const auto v2 = projectPoint(to, cameraPosition, cameraRotation);

    // Do not draw line, if both points are outside the camera view range
    if ((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getY() > 1) && (v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getY() > 1)) {
        return;
    }

    // map the points of range (-1, 1) to actual screen coordinates
    const auto x1 = static_cast<int32_t>((v1.getX() + 1) * (lfb.getResolutionX() / 2.0));
    const auto y1 = static_cast<int32_t>(lfb.getResolutionY() - (v1.getY() + 1) * (lfb.getResolutionY() / 2.0));
    const auto x2 = static_cast<int32_t>((v2.getX() + 1) * (lfb.getResolutionX() / 2.0));
    const auto y2 = static_cast<int32_t>(lfb.getResolutionY() - (v2.getY() + 1) * (lfb.getResolutionY() / 2.0));

    lineDrawer.drawLine(x1, y1, x2, y2, color);
    drawnEdgeCounter++;
}

void Graphics::drawModel(const Array<Math::Vector3D> &vertices, const Array<Math::Vector2D> &edges) {
    const auto numEdges = edges.length();
    edgeCounter += numEdges;

    for (uint32_t i = 0; i < numEdges; i++) {
        const auto edge = edges[i];
        const auto x = static_cast<int32_t>(edge.getX());
        const auto y = static_cast<int32_t>(edge.getY());

        // Do not draw edges where the vertices are the same
        if (x == y) {
            continue;
        }

        drawLine3D(vertices[x], vertices[y]);
    }
}

void Graphics::show() const {
    lfb.flush();

    if (backgroundBuffer == nullptr) {
        lfb.clear();
    } else if (Math::Vector2D(cameraPosition.getX(), cameraPosition.getY()) == Math::Vector2D(0, 0)) {
        auto source = Address<uint32_t>(backgroundBuffer);
        lfb.getBuffer().copyRange(source, lfb.getResolutionY() * lfb.getPitch());
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

void Graphics::setColor(const Graphic::Color &color) {
    Graphics::color = color;
}

Graphic::Color Graphics::getColor() const {
    return color;
}

void Graphics::saveCurrentStateAsBackground() {
    if (backgroundBuffer == nullptr) {
        backgroundBuffer = new uint8_t[lfb.getPitch() * lfb.getResolutionY()];
    }

    Address<uint32_t>(backgroundBuffer).copyRange(lfb.getBuffer(), lfb.getPitch() * lfb.getResolutionY());
}

void Graphics::clearBackground() {
    delete backgroundBuffer;
    backgroundBuffer = nullptr;
}

void Graphics::clear(const Graphic::Color &color) {
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

void Graphics::update() {
    auto &camera = game.getCurrentScene().getCamera();
    cameraPosition = camera.getPosition();
    cameraRotation = camera.getRotation();
}

uint8_t Graphics::getCharWidth() const {
    return font.getCharWidth();
}

uint8_t Graphics::getCharHeight() const {
    return font.getCharHeight();
}

uint8_t Graphics::getCharWidthSmall() const {
    return fontSmall.getCharWidth();
}

uint8_t Graphics::getCharHeightSmall() const {
    return fontSmall.getCharHeight();
}

void Graphics::resetCounters() {
    edgeCounter = 0;
    drawnEdgeCounter = 0;
}

}
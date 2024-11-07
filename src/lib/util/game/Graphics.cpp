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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Graphics.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/math/Vector2D.h"
#include "Game.h"
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Scene.h"
#include "lib/util/math/Math.h"
#include "GameManager.h"

namespace Util::Game {

Graphics::Graphics(const Util::Graphic::LinearFrameBuffer &lfb, Game &game, double scaleFactor) :
        game(game), bufferedLfb(lfb, scaleFactor), pixelDrawer(bufferedLfb), lineDrawer(pixelDrawer), stringDrawer(pixelDrawer),
        transformation((bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ? bufferedLfb.getResolutionY() : bufferedLfb.getResolutionX()) / 2),
        offsetX(transformation + (bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ? (bufferedLfb.getResolutionX() - bufferedLfb.getResolutionY()) / 2 : 0)),
        offsetY(transformation + (bufferedLfb.getResolutionY() > bufferedLfb.getResolutionX() ? (bufferedLfb.getResolutionY() - bufferedLfb.getResolutionX()) / 2 : 0)) {
    GameManager::absoluteResolution = Math::Vector2D(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY());
    GameManager::relativeResolution = Math::Vector2D(bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ? (double) bufferedLfb.getResolutionX() / bufferedLfb.getResolutionY() : 1,
                                                     bufferedLfb.getResolutionY() > bufferedLfb.getResolutionX() ? (double) bufferedLfb.getResolutionY() / bufferedLfb.getResolutionX() : 1);
    GameManager::transformation = transformation;
    lfb.clear();
}

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

void Graphics::drawString(const Graphic::Font &font, const Math::Vector2D &position, const String &string) const {
    drawString(font, position, static_cast<const char*>(string));
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

void Graphics::drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const String &string) const {
    drawString2D(font, position, static_cast<const char*>(string));
}

void Graphics::drawImage2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, const Math::Vector2D &scale, double rotationAngle) const {
    bool notScaled = Math::equals(scale.getX(), 1, 0.00001) && Math::equals(scale.getY(), 1, 0.00001);
    bool notRotated = Math::equals(rotationAngle, 0, 0.00001);

    if (notScaled && notRotated) {
        drawImageDirect2D(position, image, flipX, alpha);
    } else if (notRotated) {
        drawImageScaled2D(position, image, flipX, alpha, scale);
    } else if (notScaled) {
        drawImageRotated2D(position, image, flipX, alpha, rotationAngle);
    } else {
        drawImageScaledAndRotated2D(position, image, flipX, alpha, scale, rotationAngle);
    }
}

// Based on https://en.wikipedia.org/wiki/3D_projection#Perspective_projection
Math::Vector2D Graphics::projectPoint(const Math::Vector3D &vertex, const Math::Vector3D &cameraPosition, const Math::Vector3D &cameraRotation) {
    const auto unitVector = Math::Vector3D(0, 0, 1);
    const auto lineDirection = unitVector.rotate(cameraRotation);
    const auto distanceToClosestPointOnLine = (vertex - cameraPosition).dot(lineDirection);

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
    const auto x1 = static_cast<int32_t>((v1.getX() + 1) * (bufferedLfb.getResolutionX() / 2.0));
    const auto y1 = static_cast<int32_t>(bufferedLfb.getResolutionY() - (v1.getY() + 1) * (bufferedLfb.getResolutionY() / 2.0));
    const auto x2 = static_cast<int32_t>((v2.getX() + 1) * (bufferedLfb.getResolutionX() / 2.0));
    const auto y2 = static_cast<int32_t>(bufferedLfb.getResolutionY() - (v2.getY() + 1) * (bufferedLfb.getResolutionY() / 2.0));

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
    bufferedLfb.flush();

    if (backgroundBuffer == nullptr) {
        bufferedLfb.clear();
    } else if (Math::Vector2D(cameraPosition.getX(), cameraPosition.getY()) == Math::Vector2D(0, 0)) {
        auto source = Address<uint32_t>(backgroundBuffer);
        bufferedLfb.getBuffer().copyRange(source, bufferedLfb.getResolutionY() * bufferedLfb.getPitch());
    } else {
        auto pitch = bufferedLfb.getPitch();
        auto colorDepthDivisor = (bufferedLfb.getColorDepth() == 15 ? 16 : bufferedLfb.getColorDepth()) / 8;
        auto xOffset = static_cast<uint32_t>(game.getCurrentScene().getCamera().getPosition().getX() * static_cast<uint32_t>(pitch / colorDepthDivisor)) % pitch;
        xOffset -= xOffset % colorDepthDivisor;

        for (uint32_t i = 0; i < bufferedLfb.getResolutionY(); i++) {
            auto yOffset = pitch * i;

            auto source = Address<uint32_t>(backgroundBuffer + yOffset + xOffset);
            auto target = bufferedLfb.getBuffer().add(yOffset);
            target.copyRange(source, pitch - xOffset);

            source = Address<uint32_t>(backgroundBuffer + yOffset);
            target = bufferedLfb.getBuffer().add(yOffset + (pitch - xOffset));
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
        backgroundBuffer = new uint8_t[bufferedLfb.getPitch() * bufferedLfb.getResolutionY()];
    }

    Address<uint32_t>(backgroundBuffer).copyRange(bufferedLfb.getBuffer(), bufferedLfb.getPitch() * bufferedLfb.getResolutionY());
}

void Graphics::clearBackground() {
    delete backgroundBuffer;
    backgroundBuffer = nullptr;
}

void Graphics::clear(const Graphic::Color &color) {
    if (color == Util::Graphic::Colors::BLACK) {
        bufferedLfb.clear();
    } else {
        for (uint32_t i = 0; i < bufferedLfb.getResolutionX(); i++) {
            for (uint32_t j = 0; j < bufferedLfb.getResolutionY(); j++) {
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

void Graphics::resetCounters() {
    edgeCounter = 0;
    drawnEdgeCounter = 0;
}

void Graphics::drawImageDirect2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha) const {
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);

    if (xPixelOffset + image.getWidth() < 0 || xPixelOffset > bufferedLfb.getResolutionX() || yPixelOffset - image.getHeight() > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    for (uint32_t i = 0; i < image.getHeight(); i++) {
        for (uint32_t j = 0; j < image.getWidth(); j++) {
            const auto &pixel = pixelBuffer[i * image.getWidth() + (flipX ? image.getWidth() - j : j)];
            pixelDrawer.drawPixel(xPixelOffset + j, yPixelOffset - i, pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageScaled2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, const Math::Vector2D &scale) const {
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int16_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int16_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);

    const auto scaledWidth = static_cast<uint16_t>(image.getWidth() * scale.getX());
    const auto scaledHeight = static_cast<uint16_t>(image.getHeight() * scale.getY());

    if (xPixelOffset + scaledWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() || yPixelOffset - scaledHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    const auto factorX = static_cast<double>(scaledWidth) / image.getWidth();
    const auto factorY = static_cast<double>(scaledHeight) / image.getHeight();

    for (uint32_t i = 0; i < scaledHeight; i++) {
        for (uint32_t j = 0; j < scaledWidth; j++) {
            const auto imageX = static_cast<uint16_t>(j / factorX);
            const auto imageY = static_cast<uint16_t>(i / factorY);

            const auto &pixel = pixelBuffer[imageY * image.getWidth() + (flipX ? (image.getWidth() - imageX) : imageX)];
            pixelDrawer.drawPixel(xPixelOffset + j, yPixelOffset - i, pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageRotated2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, double rotationAngle) const {
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);

    const auto centerX = image.getWidth() / 2.0;
    const auto centerY = image.getHeight() / 2.0;
    const auto rotatedHeight = static_cast<uint16_t>(image.getWidth() * Math::absolute(Math::sine(rotationAngle)) + image.getHeight() * Math::absolute(Math::cosine(rotationAngle)));
    const auto rotatedWidth = static_cast<uint16_t>(image.getWidth() * Math::absolute(Math::cosine(rotationAngle)) + image.getHeight() * Math::absolute(Math::sine(rotationAngle)));

    if (xPixelOffset + rotatedWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() || yPixelOffset - rotatedHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    const auto yOffset = rotatedHeight - image.getHeight();
    const auto xOffset = rotatedWidth - image.getWidth();

    for (int32_t i = -yOffset; i < rotatedHeight; i++) {
        for (int32_t j = -xOffset; j < rotatedWidth; j++) {
            const auto imageX = static_cast<uint16_t>(centerX + (j - centerX) * Math::cosine(rotationAngle) + (i - centerY) * Math::sine(rotationAngle));
            const auto imageY = static_cast<uint16_t>(centerY - (j - centerX) * Math::sine(rotationAngle) + (i - centerY) * Math::cosine(rotationAngle));
            if (imageX >= image.getWidth() || imageY >= image.getHeight()) {
                continue;
            }

            const auto &pixel = pixelBuffer[imageY * image.getWidth() + (flipX ? (image.getWidth() - imageX) : imageX)];
            pixelDrawer.drawPixel(xPixelOffset + j, yPixelOffset - i, pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageScaledAndRotated2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha, const Math::Vector2D &scale, double rotationAngle) const {
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - cameraPosition.getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>((-position.getY() + cameraPosition.getY()) * transformation + offsetY);

    const auto scaledWidth = static_cast<uint16_t>(image.getWidth() * scale.getX());
    const auto scaledHeight = static_cast<uint16_t>(image.getHeight() * scale.getY());

    const auto centerX = scaledWidth / 2.0;
    const auto centerY = scaledHeight / 2.0;
    const auto rotatedHeight = static_cast<uint16_t>(scaledWidth * Math::absolute(Math::sine(rotationAngle)) + scaledHeight * Math::absolute(Math::cosine(rotationAngle)));
    const auto rotatedWidth = static_cast<uint16_t>(scaledWidth * Math::absolute(Math::cosine(rotationAngle)) + scaledHeight * Math::absolute(Math::sine(rotationAngle)));

    if (xPixelOffset + rotatedWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() || yPixelOffset - rotatedHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    // Scale image into new buffer
    auto *scaledPixelBuffer = new Graphic::Color[scaledWidth * scaledHeight];
    double factorX = static_cast<double>(scaledWidth) / image.getWidth();
    double factorY = static_cast<double>(scaledHeight) / image.getHeight();

    for (int i = 0; i < scaledHeight; i++) {
        for (int j = 0; j < scaledWidth; j++) {
            auto imageX = static_cast<uint16_t>(j / factorX);
            auto imageY = static_cast<uint16_t>(i / factorY);
            scaledPixelBuffer[scaledWidth * i + j] = pixelBuffer[image.getWidth() * imageY + imageX];
        }
    }

    // Draw rotated image from scaled buffer
    const auto yOffset = rotatedHeight - scaledHeight;
    const auto xOffset = rotatedWidth - scaledWidth;

    for (int32_t i = -yOffset; i < scaledHeight + yOffset; i++) {
        for (int32_t j = -xOffset; j < scaledWidth + xOffset; j++) {
            const auto imageX = static_cast<uint16_t>(centerX + (j - centerX) * Math::cosine(rotationAngle) + (i - centerY) * Math::sine(rotationAngle));
            const auto imageY = static_cast<uint16_t>(centerY - (j - centerX) * Math::sine(rotationAngle) + (i - centerY) * Math::cosine(rotationAngle));
            if (imageX >= scaledWidth || imageY >= scaledHeight) {
                continue;
            }

            const auto &pixel = scaledPixelBuffer[imageY * scaledWidth + (flipX ? (scaledWidth - imageX) : imageX)];
            pixelDrawer.drawPixel(xPixelOffset + j, yPixelOffset - i, pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }

    delete[] scaledPixelBuffer;
}

}
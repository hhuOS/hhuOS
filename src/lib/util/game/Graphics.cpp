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
#include "lib/util/game/Camera.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/base/Address.h"
#include "lib/util/game/Scene.h"
#include "lib/util/math/Math.h"
#include "GameManager.h"
#include "Game.h"
#include "lib/util/game/3d/Scene.h"
#include "lib/util/base/System.h"

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

void Graphics::drawLineDirectAbsolute(uint16_t fromX, uint16_t fromY, uint16_t toX, uint16_t toY) const {
    lineDrawer.drawLine(fromX, fromY, toX, toY, color);
}

void Graphics::drawRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const {
    drawLineDirectAbsolute(posX, posY, posX + width, posY);
    drawLineDirectAbsolute(posX, posY + height, posX + width, posY + height);
    drawLineDirectAbsolute(posX, posY, posX, posY + height);
    drawLineDirectAbsolute(posX + width, posY, posX + width, posY + height);
}

void Graphics::drawSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const {
    drawRectangleDirectAbsolute(posX, posY, size, size);
}

void Graphics::fillRectangleDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height) const {
    const uint16_t endX = posX + width;
    const uint16_t endY = posY + height;

    for (uint16_t i = posY; i < endY; i++) {
        lineDrawer.drawLine(posX, i, endX, i, color);
    }
}

void Graphics::fillSquareDirectAbsolute(uint16_t posX, uint16_t posY, uint16_t size) const {
    fillRectangleDirectAbsolute(posX, posY, size, size);
}

void Graphics::drawStringDirectAbsolute(const Graphic::Font &font, uint16_t posX, uint16_t posY, const char *string) const {
    stringDrawer.drawString(font, posX, posY, string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics::drawStringDirectAbsolute(const Graphic::Font &font, uint16_t posX, uint16_t posY, const String &string) const {
    drawStringDirectAbsolute(font, posX, posY, static_cast<const char*>(string));
}

void Graphics::drawLineDirect(const Math::Vector2D &from, const Math::Vector2D &to) const {
    drawLineDirectAbsolute(static_cast<uint16_t>(from.getX() * transformation + offsetX),
                           static_cast<uint16_t>(-from.getY() * transformation + offsetY),
                           static_cast<uint16_t>(to.getX() * transformation + offsetX),
                           static_cast<uint16_t>(-to.getY() * transformation + offsetY));
}

void Graphics::drawRectangleDirect(const Math::Vector2D &position, double width, double height) const {
    const auto x = position.getX();
    const auto y = position.getY();

    drawLineDirect(position, Math::Vector2D(x + width, y));
    drawLineDirect(Math::Vector2D(x, y + height), Math::Vector2D(x + width, y + height));
    drawLineDirect(position, Math::Vector2D(x, y + height));
    drawLineDirect(Math::Vector2D(x + width, y), Math::Vector2D(x + width, y + height));
}

void Graphics::drawSquareDirect(const Math::Vector2D &position, double size) const {
    drawRectangleDirect(position, size, size);
}

void Graphics::fillRectangleDirect(const Math::Vector2D &position, double width, double height) const {
    auto startX = static_cast<int32_t>(position.getX() * transformation + offsetX);
    auto endX = static_cast<int32_t>((position.getX() + width) * transformation + offsetX);
    auto startY = static_cast<int32_t>(-position.getY() * transformation + offsetY);
    auto endY = static_cast<int32_t>(-(position.getY() + height) * transformation + offsetY);

    if (startY > endY) {
        int32_t temp = startY;
        startY = endY;
        endY = temp;
    }

    for (int32_t i = startY; i < endY; i++) {
        lineDrawer.drawLine(startX, i, endX, i, color);
    }
}

void Graphics::fillSquareDirect(const Math::Vector2D &position, double size) const {
    fillRectangleDirect(position, size, size);
}

void Graphics::drawStringDirect(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    drawStringDirectAbsolute(font,
                             static_cast<uint16_t>(position.getX() * transformation + offsetX),
                             static_cast<uint16_t>(-position.getY() * transformation + offsetY),
                             string);
}

void Graphics::drawStringDirect(const Graphic::Font &font, const Math::Vector2D &position, const String &string) const {
    drawStringDirect(font, position, static_cast<const char*>(string));
}

/***** 2D drawing functions, respecting the camera position *****/

void Graphics::drawLine2D(const Math::Vector2D &from, const Math::Vector2D &to) const {
    lineDrawer.drawLine(static_cast<int32_t>((from.getX() - camera.getPosition().getX()) * transformation + offsetX),
                        static_cast<int32_t>(-(from.getY() - camera.getPosition().getY()) * transformation + offsetY),
                        static_cast<int32_t>((to.getX() - camera.getPosition().getX()) * transformation + offsetX),
                        static_cast<int32_t>(-(to.getY() - camera.getPosition().getY()) * transformation + offsetY), color);
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
    auto startX = static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    auto endX = static_cast<int32_t>((position.getX() + width - camera.getPosition().getX()) * transformation + offsetX);
    auto startY = static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY);
    auto endY = static_cast<int32_t>(-(position.getY() + height - camera.getPosition().getY()) * transformation + offsetY);

    if (startY > endY) {
        int32_t temp = startY;
        startY = endY;
        endY = temp;
    }

    for (int32_t i = startY; i < endY; i++) {
        lineDrawer.drawLine(startX, i, endX, i, color);
    }
}

void Graphics::drawString2D(const Graphic::Font &font, const Math::Vector2D &position, const char *string) const {
    stringDrawer.drawString(font,
        static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX),
        static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY),
        string, color, Util::Graphic::Colors::INVISIBLE);
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

void Graphics::drawModel(const D3::Model &model) {
    if (!glEnabled()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Graphics: OpenGL is not enabled");
    }

    const auto &position = model.getPosition();
    const auto &rotation = model.getRotation();
    const auto &scale = model.getScale();
    const auto &vertices = model.getVertices();
    const auto &normals = model.getVertexNormals();
    // const auto &textureCoordinates = model.getVertexTextures();
    const auto &vertexDrawOrder = model.getVertexDrawOrder();
    const auto &normalDrawOrder = model.getNormalDrawOrder();
    // const auto &textureDrawOrder = model.getTextureDrawOrder();

    glPushMatrix();
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);
    glScalef(scale.getX(), scale.getY(), scale.getZ());
    glColor3f(color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f);
    glBegin(GL_TRIANGLES);

    for (uint32_t i = 0; i < vertexDrawOrder.length(); i++) {
        if (normalDrawOrder.length() > i) {
            const auto &normal = normals[normalDrawOrder[i]];
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
        }

        const auto &vertex = vertices[vertexDrawOrder[i]];
        glVertex3f(vertex.getX(), vertex.getY(), vertex.getZ());
    }

    glEnd();
    glPopMatrix();
}
void Graphics::initializeGl() {
    glBuffer = ZB_open(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY(), ZB_MODE_RGBA, reinterpret_cast<void*>(bufferedLfb.getBuffer().get()));
    glInit(glBuffer);

    const auto width = static_cast<GLdouble>(bufferedLfb.getResolutionX());
    const auto height = static_cast<GLdouble>(bufferedLfb.getResolutionY());

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up the view port to match the resolution of the frame buffer
    glViewport(0, 0, bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY());

    // Set the camera perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,             // Camera angle
                   width / height,   // Width to height ratio
                   1.0,              // Near z-clipping coordinate (Object nearer than that will not be drawn)
                   10000.0);         // Far z-clipping coordinate (Object farther than that will not be drawn)

    // Enable required OpenGL features
    glEnable(GL_DEPTH_TEST); // Depth testing to make sure the shapes are drawn in the correct order
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_NORMALIZE); // Automatically normalize normals

    // Set shade model and render style
    const auto &scene = reinterpret_cast<D3::Scene&>(game.getCurrentScene());
    glShadeModel(scene.getGlShadeModel());
    glPolygonMode(GL_FRONT_AND_BACK, scene.getGlRenderStyle());
}

void Graphics::closeGl() {
    if (!glEnabled()) {
        return;
    }

    glClose();
    ZB_close(glBuffer);
    glBuffer = nullptr;
}

bool Graphics::glEnabled() const {
    return glBuffer != nullptr;
}

void Graphics::show() const {
    bufferedLfb.flush();

    if (glBuffer != nullptr) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if (backgroundBuffer == nullptr) {
        bufferedLfb.clear();
    } else if (Math::Vector2D(camera.getPosition().getX(), camera.getPosition().getY()) == Math::Vector2D(0, 0)) {
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
    if (glBuffer != nullptr) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

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
    camera = game.getCurrentScene().getCamera();

    if (glEnabled()) {
        // Set drawing perspective
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Set camera position and rotation
        glRotatef(camera.getRotation().getZ(), worldUpVecotor.getX(), worldUpVecotor.getY(), worldUpVecotor.getZ()); // Yaw
        glRotatef(camera.getRotation().getY(), camera.getRightVector().getX(), camera.getRightVector().getY(), camera.getRightVector().getZ()); // Pitch
        glTranslatef(-camera.getPosition().getX(), -camera.getPosition().getY(), -camera.getPosition().getZ()); // Position

        // Lights
        auto &scene = reinterpret_cast<D3::Scene &>(game.getCurrentScene());
        GLfloat ambientColor[4] = { scene.getAmbientLight().getRed() / 255.0f, scene.getAmbientLight().getGreen() / 255.0f, scene.getAmbientLight().getBlue() / 255.0f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        for (uint32_t i = 0; i < 16; i++) {
            if (scene.hasLight(i)) {
                const auto &light = scene.getLight(i);
                GLfloat position[4] = {static_cast<GLfloat>(light.getPosition().getX()), static_cast<GLfloat>(light.getPosition().getY()),
                                       static_cast<GLfloat>(light.getPosition().getZ()), static_cast<GLfloat>(light.getType())};
                GLfloat diffuse[4] = {light.getDiffuseColor().getRed() / 255.0f, light.getDiffuseColor().getGreen() / 255.0f, light.getDiffuseColor().getBlue() / 255.0f, 1.0f};
                GLfloat specular[4] = {light.getSpecularColor().getRed() / 255.0f, light.getSpecularColor().getGreen() / 255.0f, light.getSpecularColor().getBlue() / 255.0f, 1.0f};

                glLightfv(GL_LIGHT0 + i, GL_POSITION, position);
                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
                glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
            }
        }
    }
}

void Graphics::drawImageDirect2D(const Math::Vector2D &position, const Graphic::Image &image, bool flipX, double alpha) const {
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

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
    const auto xPixelOffset = static_cast<int16_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int16_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

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
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

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
    const auto xPixelOffset = static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

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

/**
 * Taken from https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
 */
void Graphics::gluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar) const {
    const GLdouble pi = Util::Math::PI;
    GLdouble fW, fH;

    fH = Util::Math::tangent(fovY / 360 * pi) * zNear;
    fW = fH * aspect;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

}
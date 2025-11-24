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

#include "Graphics.h"

#include "util/base/Address.h"
#include "util/base/Panic.h"
#include "util/graphic/Image.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/math/Math.h"
#include "util/math/Vector2.h"
#include "pulsar/Camera.h"
#include "pulsar/Game.h"
#include "pulsar/Scene.h"
#include "pulsar/3d/Orientation.h"
#include "pulsar/3d/Scene.h"
#include "pulsar/3d/Light.h"
#include "pulsar/3d/Model.h"
#include "pulsar/3d/Texture.h"

namespace Pulsar {

Graphics::Graphics(const Util::Graphic::LinearFrameBuffer &lfb, const double scaleFactor) :
        bufferedLfb(lfb, scaleFactor),
        transformation((bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ?
            bufferedLfb.getResolutionY() : bufferedLfb.getResolutionX()) / 2),
        offsetX(transformation + (bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ?
            (bufferedLfb.getResolutionX() - bufferedLfb.getResolutionY()) / 2 : 0)),
        offsetY(transformation + (bufferedLfb.getResolutionY() > bufferedLfb.getResolutionX() ?
            (bufferedLfb.getResolutionY() - bufferedLfb.getResolutionX()) / 2 : 0)),
        dimensions(bufferedLfb.getResolutionX() > bufferedLfb.getResolutionY() ?
            static_cast<double>(bufferedLfb.getResolutionX()) / bufferedLfb.getResolutionY() : 1,
            bufferedLfb.getResolutionY() > bufferedLfb.getResolutionX() ?
            static_cast<double>(bufferedLfb.getResolutionY()) / bufferedLfb.getResolutionX() : 1) {}

Graphics::~Graphics() {
    delete backgroundBuffer;
}

/***** Basic functions to draw directly on the screen ******/

void Graphics::drawLineDirectAbsolute(const uint16_t fromX, const uint16_t fromY,
                                      const uint16_t toX, const uint16_t toY) const
{
    if (isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: Drawing with absolute coordinates is not supported in OpenGL mode!");
    }

    bufferedLfb.drawLine(fromX, fromY, toX, toY, color);
}

void Graphics::drawRectangleDirectAbsolute(const uint16_t posX, const uint16_t posY,
    const uint16_t width, const uint16_t height) const
{
    drawLineDirectAbsolute(posX, posY, posX + width, posY);
    drawLineDirectAbsolute(posX, posY + height, posX + width, posY + height);
    drawLineDirectAbsolute(posX, posY, posX, posY + height);
    drawLineDirectAbsolute(posX + width, posY, posX + width, posY + height);
}

void Graphics::drawSquareDirectAbsolute(const uint16_t posX, const uint16_t posY, const uint16_t size) const {
    drawRectangleDirectAbsolute(posX, posY, size, size);
}

void Graphics::fillRectangleDirectAbsolute(const uint16_t posX, const uint16_t posY,
    const uint16_t width, const uint16_t height) const
{
    const uint16_t endX = posX + width;
    const uint16_t endY = posY + height;

    for (uint16_t i = posY; i < endY; i++) {
        bufferedLfb.drawLine(posX, i, endX, i, color);
    }
}

void Graphics::fillSquareDirectAbsolute(const uint16_t posX, const uint16_t posY, const uint16_t size) const {
    fillRectangleDirectAbsolute(posX, posY, size, size);
}

void Graphics::drawStringDirectAbsolute(const uint16_t posX, const uint16_t posY, const char *string) const {
    if (isGlEnabled()) {
        glDrawText(reinterpret_cast<const GLubyte*>(string), posX, posY, color.getRGB32());
    } else {
        bufferedLfb.drawString(Util::Graphic::Fonts::TERMINAL_8x8, posX, posY, string,
            color, Util::Graphic::Colors::INVISIBLE);
    }
}

void Graphics::drawStringDirectAbsolute(const uint16_t posX, const uint16_t posY, const Util::String &string) const {
    drawStringDirectAbsolute(posX, posY, static_cast<const char*>(string));
}

void Graphics::drawLineDirect(const Util::Math::Vector2<double> &from, const Util::Math::Vector2<double> &to) const {
    if (isGlEnabled()) {
        gluPrepareDirectDraw(D3::Scene::LINES);

        glColor3f(static_cast<float>(color.getRed()) / 255,
            static_cast<float>(color.getGreen()) / 255,
            static_cast<float>(color.getBlue()) / 255);
        glBegin(GL_LINES);
        glVertex2f(static_cast<float>(from.getX()), static_cast<float>(from.getY()));
        glVertex2f(static_cast<float>(to.getX()), static_cast<float>(to.getY()));
        glEnd();

        gluFinishDirectDraw();
    } else {
        drawLineDirectAbsolute(static_cast<uint16_t>(from.getX() * transformation + offsetX),
                               static_cast<uint16_t>(-from.getY() * transformation + offsetY),
                               static_cast<uint16_t>(to.getX() * transformation + offsetX),
                               static_cast<uint16_t>(-to.getY() * transformation + offsetY));
    }
}

void Graphics::drawRectangleDirect(const Util::Math::Vector2<double> &position,
    const Util::Math::Vector2<double> &size) const
{
    const auto x = position.getX();
    const auto y = position.getY();
    const auto width = size.getX();
    const auto height = size.getY();

    if (isGlEnabled()) {
        gluPrepareDirectDraw(D3::Scene::LINES);

        glColor3f(static_cast<float>(color.getRed()) / 255,
            static_cast<float>(color.getGreen()) / 255,
            static_cast<float>(color.getBlue()) / 255);
        glBegin(GL_QUADS);

        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);

        glEnd();

        gluFinishDirectDraw();
    } else {
        drawLineDirect(position, Util::Math::Vector2<double>(x + width, y));
        drawLineDirect(Util::Math::Vector2<double>(x, y + height),
            Util::Math::Vector2<double>(x + width, y + height));
        drawLineDirect(position, Util::Math::Vector2<double>(x, y + height));
        drawLineDirect(Util::Math::Vector2<double>(x + width, y),
            Util::Math::Vector2<double>(x + width, y + height));
    }
}

void Graphics::drawSquareDirect(const Util::Math::Vector2<double> &position, const double size) const {
    drawRectangleDirect(position, Util::Math::Vector2<double>(size, size));
}

void Graphics::fillRectangleDirect(const Util::Math::Vector2<double> &position,
    const Util::Math::Vector2<double> &size) const
{
    const auto x = position.getX();
    const auto y = position.getY();
    const auto width = size.getX();
    const auto height = size.getY();

    if (isGlEnabled()) {
        gluPrepareDirectDraw(D3::Scene::FILL);

        glColor3f(static_cast<float>(color.getRed()) / 255,
            static_cast<float>(color.getGreen()) / 255,
            static_cast<float>(color.getBlue()) / 255);
        glBegin(GL_QUADS);

        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);

        glEnd();
        gluFinishDirectDraw();
    } else {
        const auto startX = static_cast<int32_t>(x * transformation + offsetX);
        const auto endX = static_cast<int32_t>((x + width) * transformation + offsetX);
        auto startY = static_cast<int32_t>(-y * transformation + offsetY);
        auto endY = static_cast<int32_t>(-(y + height) * transformation + offsetY);

        if (startY > endY) {
            const int32_t temp = startY;
            startY = endY;
            endY = temp;
        }

        for (int32_t i = startY; i < endY; i++) {
            bufferedLfb.drawLine(startX, i, endX, i, color);
        }
    }
}

void Graphics::fillSquareDirect(const Util::Math::Vector2<double> &position, const double size) const {
    fillRectangleDirect(position, Util::Math::Vector2<double>(size, size));
}

void Graphics::drawStringDirect(const Util::Math::Vector2<double> &position, const char *string) const {
    if (isGlEnabled()) {
        glDrawText(reinterpret_cast<const GLubyte*>(string),
                 static_cast<uint16_t>(position.getX() * transformation + offsetX),
                 static_cast<uint16_t>(-position.getY() * transformation + offsetY),
                 color.getRGB32());
    } else {
        drawStringDirectAbsolute(static_cast<uint16_t>(position.getX() * transformation + offsetX),
                                 static_cast<uint16_t>(-position.getY() * transformation + offsetY),
                                 string);
    }
}

void Graphics::drawStringDirect(const Util::Math::Vector2<double> &position, const Util::String &string) const {
    drawStringDirect(position, static_cast<const char*>(string));
}

/***** 2D drawing functions, respecting the camera position *****/

void Graphics::drawLine2D(const Util::Math::Vector2<double> &from, const Util::Math::Vector2<double> &to) const {
    if (isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 2D drawing functions are not supported in OpenGL mode!");
    }

    bufferedLfb.drawLine(
        static_cast<int32_t>((from.getX() - camera.getPosition().getX()) * transformation + offsetX),
        static_cast<int32_t>(-(from.getY() - camera.getPosition().getY()) * transformation + offsetY),
        static_cast<int32_t>((to.getX() - camera.getPosition().getX()) * transformation + offsetX),
        static_cast<int32_t>(-(to.getY() - camera.getPosition().getY()) * transformation + offsetY),
        color);
}

void Graphics::drawPolygon2D(const Util::Math::Vector2<double> &position,
    const Util::Array<Util::Math::Vector2<double>> &vertices) const
{
    for (uint32_t i = 0; i < vertices.length() - 1; i++) {
        drawLine2D(vertices[i] + position, vertices[i + 1] + position);
    }

    drawLine2D(vertices[vertices.length() - 1] + position, vertices[0] + position);
}

void Graphics::drawRectangle2D(const Util::Math::Vector2<double> &position,
    const Util::Math::Vector2<double> &size) const
{
    const auto x = position.getX();
    const auto y = position.getY();
    const auto width = size.getX();
    const auto height = size.getY();

    drawLine2D(position, Util::Math::Vector2<double>(x + width, y));
    drawLine2D(Util::Math::Vector2<double>(x, y - height), Util::Math::Vector2<double>(x + width, y - height));
    drawLine2D(position, Util::Math::Vector2<double>(x, y - height));
    drawLine2D(Util::Math::Vector2<double>(x + width, y), Util::Math::Vector2<double>(x + width, y - height));
}

void Graphics::drawSquare2D(const Util::Math::Vector2<double> &position, const double size) const {
    drawRectangle2D(position, Util::Math::Vector2<double>(size, size));
}

void Graphics::fillRectangle2D(const Util::Math::Vector2<double> &position,
    const Util::Math::Vector2<double> &size) const
{
    if (isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 2D drawing functions are not supported in OpenGL mode!");
    }

    const auto width = size.getX();
    const auto height = size.getY();
    const auto startX = static_cast<int32_t>(
        (position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto endX = static_cast<int32_t>(
        (position.getX() + width - camera.getPosition().getX()) * transformation + offsetX);
    auto startY = static_cast<int32_t>(
        -(position.getY() - camera.getPosition().getY()) * transformation + offsetY);
    auto endY = static_cast<int32_t>(
        -(position.getY() + height - camera.getPosition().getY()) * transformation + offsetY);

    if (startY > endY) {
        const int32_t temp = startY;
        startY = endY;
        endY = temp;
    }

    for (int32_t i = startY; i < endY; i++) {
        bufferedLfb.drawLine(startX, i, endX, i, color);
    }
}

void Graphics::fillSquare2D(const Util::Math::Vector2<double> &position, const double size) const {
    fillRectangle2D(position, Util::Math::Vector2<double>(size, size));
}

void Graphics::drawString2D(const Util::Math::Vector2<double> &position, const char *string) const {
    if (isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 2D drawing functions are not supported in OpenGL mode!");
    }

    bufferedLfb.drawString(Util::Graphic::Fonts::TERMINAL_8x8,
        static_cast<int32_t>((position.getX() - camera.getPosition().getX()) * transformation + offsetX),
        static_cast<int32_t>(-(position.getY() - camera.getPosition().getY()) * transformation + offsetY),
        string, color, Util::Graphic::Colors::INVISIBLE);
}

void Graphics::drawString2D(const Util::Math::Vector2<double> &position, const Util::String &string) const {
    drawString2D(position, static_cast<const char*>(string));
}

void Graphics::drawImage2D(const Util::Math::Vector2<double> &position, const Util::Graphic::Image &image,
    const bool flipX, const double alpha, const Util::Math::Vector2<double> &scale, const double rotationAngle) const
{
    if (isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 2D drawing functions are not supported in OpenGL mode!");
    }

    const auto notScaled = Util::Math::equals(scale.getX(), 1, 0.00001) &&
        Util::Math::equals(scale.getY(), 1, 0.00001);
    const auto notRotated = Util::Math::equals(rotationAngle, 0, 0.00001);

    if (notScaled && notRotated) {
        drawImageDirect2D(position, image, flipX, alpha);
    } else if (notRotated) {
        drawImageScaled2D(position, image, flipX, alpha, scale);
    } else if (notScaled) {
        drawImageRotated2D(position, image, flipX, alpha, Util::Math::toRadians(rotationAngle));
    } else {
        drawImageScaledAndRotated2D(position, image, flipX, alpha, scale, Util::Math::toRadians(rotationAngle));
    }
}

void Graphics::drawModel3D(const D3::Model &model) const {
    if (!isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 3D drawing functions are only supported in OpenGL mode!");
    }

    const auto &position = model.getPosition();
    const auto &rotation = model.getRotation();
    const auto &scale = model.getScale();

    glPushMatrix();

    // Translate, Rotate, Scale
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);
    glScalef(scale.getX(), scale.getY(), scale.getZ());

    // Set color
    if (model.getTexture().getTextureID() == 0) {
        glColor3f(static_cast<float>(color.getRed()) / 255,
            static_cast<float>(color.getGreen()) / 255,
            static_cast<float>(color.getBlue()) / 255);
    }

    listModel3D(model);

    glPopMatrix();
}

void Graphics::drawRectangle3D(const Util::Math::Vector3<double> &position, const Util::Math::Vector2<double> &size,
    const Util::Math::Vector3<double> &rotation, const D3::Texture &texture) const
{
    if (!isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 3D drawing functions are only supported in OpenGL mode!");
    }

    glPushMatrix();

    // Translate, Rotate
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);

    // Set color
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);

    listRectangle3D(size, texture);

    glPopMatrix();
}

void Graphics::drawCustomShape3D(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &scale,
    const Util::Math::Vector3<double> &rotation, const Util::Array<Util::Math::Vector3<double>> &vertices) const
{
    if (!isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 3D drawing functions are only supported in OpenGL mode!");
    }

    glPushMatrix();

    // Translate, Rotate, Scale
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);
    glScalef(scale.getX(), scale.getY(), scale.getZ());

    // Set color
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);

    // Draw shape (consisting of triangles)
    listCustomShape3D(vertices);

    glPopMatrix();
}

void Graphics::drawCuboid3D(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &size,
    const Util::Math::Vector3<double> &rotation, const D3::Texture &texture) const
{
    if (!isGlEnabled()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION,
            "Graphics: 3D drawing functions are only supported in OpenGL mode!");
    }

    glPushMatrix();

    // Translate, Rotate
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);

    // Set color
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);

    listCuboid3D(size, texture);

    glPopMatrix();
}

void Graphics::drawList3D(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &scale,
    const Util::Math::Vector3<double> &rotation, const GLuint list) const
{
    glPushMatrix();

    // Translate, Rotate, Scale
    glTranslatef(position.getX(), position.getY(), position.getZ());
    glRotatef(rotation.getX(), 0.0f, 0.0f, 1.0f);
    glRotatef(rotation.getY(), 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.getZ(), 0.0f, 1.0f, 0.0f);
    glScalef(scale.getX(), scale.getY(), scale.getZ());

    // Set color
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);

    glCallList(list);

    glPopMatrix();
}

GLuint Graphics::startList3D() {
    const auto list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    return list;
}

void Graphics::endList3D() {
    glEndList();
}

void Graphics::listModel3D(const D3::Model &model) {
    const auto &vertices = model.getVertices();
    const auto &normals = model.getVertexNormals();
    const auto &textureCoordinates = model.getVertexTextures();
    const auto &vertexDrawOrder = model.getVertexDrawOrder();
    const auto &normalDrawOrder = model.getNormalDrawOrder();
    const auto &textureDrawOrder = model.getTextureDrawOrder();
    const auto textureId = model.getTexture().getTextureID();

    // Set texture or color
    if (textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLint>(textureId));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Draw triangles
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertexDrawOrder.length(); i++) {
        if (normalDrawOrder.length() > i) {
            const auto &normal = normals[normalDrawOrder[i]];
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
        }

        if (textureId != 0 && textureDrawOrder.length() > i) {
            const auto &textureCoordinate = textureCoordinates[textureDrawOrder[i]];
            glTexCoord2f(textureCoordinate.getX(), textureCoordinate.getY());
        }

        const auto &vertex = vertices[vertexDrawOrder[i]];
        glVertex3f(vertex.getX(), vertex.getY(), vertex.getZ());
    }
    glEnd();

    // Disable texture (if enabled)
    if (textureId != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

void Graphics::listCuboid3D(const Util::Math::Vector3<double> &size, const Util::Graphic::Color &color) {
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);
    listCuboid3D(size, D3::Texture());
}

void Graphics::listCuboid3D(const Util::Math::Vector3<double> &translation,
    const Util::Math::Vector3<double> &size, const Util::Graphic::Color &color)
{
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);
    listCuboid3D(translation, size, D3::Texture());
}

void Graphics::listCuboid3D(const Util::Math::Vector3<double> &size, const D3::Texture &texture) {
    const auto width = size.getX();
    const auto height = size.getY();
    const auto depth = size.getZ();
    const auto xLeft= -width / 2;
    const auto yLeft= -height / 2;
    const auto zLeft= depth / 2;
    const auto textureId = texture.getTextureID();

    // Set texture
    if (textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLint>(texture.getTextureID()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Draw cuboid
    glBegin(GL_QUADS);

    //Front-Face
    glNormal3f(0.0f, 0.0f, 1.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(xLeft, yLeft, zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex3f(xLeft + width, yLeft, zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex3f(xLeft + width, yLeft + height, zLeft);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex3f(xLeft, yLeft + height, zLeft);

    //Back-Face
    glNormal3f(0.0f, 0.0f, -1.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(xLeft, yLeft, zLeft - depth);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex3f(xLeft + width, yLeft, zLeft - depth);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex3f(xLeft + width,yLeft + height, zLeft - depth);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex3f(xLeft, yLeft + height, zLeft - depth);

    //Top-Face
    glNormal3f(0.0f ,1.0f, 0.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(xLeft, yLeft + height, zLeft - depth);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex3f(xLeft + width, yLeft + height,zLeft - depth);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex3f(xLeft + width, yLeft + height, zLeft);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex3f(xLeft, yLeft + height, zLeft);

    //Down-Face
    glNormal3f(0.0f,-1.0f,0.0f);
    if (textureId != 0) glTexCoord2f(0.0f,0.0f);
    glVertex3f(xLeft,yLeft,zLeft-depth);
    if (textureId != 0) glTexCoord2f(1.0f,0.0f);
    glVertex3f(xLeft+width,yLeft,zLeft-depth);
    if (textureId != 0) glTexCoord2f(1.0f,1.0f);
    glVertex3f(xLeft+width,yLeft,zLeft);
    if (textureId != 0) glTexCoord2f(0.0f,1.0f);
    glVertex3f(xLeft,yLeft,zLeft);

    //Left-Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(xLeft,yLeft,zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex3f(xLeft, yLeft + height, zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex3f(xLeft, yLeft + height, zLeft - depth);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex3f(xLeft, yLeft, zLeft - depth);

    //Right-Face
    glNormal3f(1.0f, 0.0f, 0.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(xLeft + width, yLeft, zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex3f(xLeft + width, yLeft + height, zLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex3f(xLeft + width, yLeft + height, zLeft - depth);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex3f(xLeft + width, yLeft, zLeft - depth);

    glEnd();

    // Disable texture (if enabled)
    if (textureId != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

void Graphics::listCuboid3D(const Util::Math::Vector3<double> &translation, const Util::Math::Vector3<double> &size,
    const D3::Texture &texture)
{
    glPushMatrix();

    glTranslatef(translation.getX(), translation.getY(), translation.getZ());
    listCuboid3D(size, texture);

    glPopMatrix();
}

void Graphics::listRectangle3D(const Util::Math::Vector2<double> &size, const Util::Graphic::Color &color) {
    glColor3f(static_cast<float>(color.getRed()) / 255,
        static_cast<float>(color.getGreen()) / 255,
        static_cast<float>(color.getBlue()) / 255);
    listRectangle3D(size, D3::Texture());
}

void Graphics::listRectangle3D(const Util::Math::Vector2<double> &size, const D3::Texture &texture) {
    const auto xLeft = -size.getX() / 2;
    const auto yLeft = -size.getY() / 2;
    const auto width = size.getX();
    const auto height = size.getY();
    const auto textureId = texture.getTextureID();

    // Set texture
    if (textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.getTextureID());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    if (textureId != 0) glTexCoord2f(0.0f, 0.0f);
    glVertex2f(xLeft, yLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 0.0f);
    glVertex2f(xLeft + width, yLeft);
    if (textureId != 0) glTexCoord2f(1.0f, 1.0f);
    glVertex2f(xLeft + width, yLeft + height);
    if (textureId != 0) glTexCoord2f(0.0f, 1.0f);
    glVertex2f(xLeft, yLeft + height);
    glEnd();

    // Disable texture (if enabled)
    if (textureId != 0) {
        glDisable(GL_TEXTURE_2D);
    }
}

void Graphics::listCustomShape3D(const Util::Array<Util::Math::Vector3<double>> &vertices) {
    // Draw shape (consisting of triangles)
    glBegin(GL_TRIANGLES);

    for (size_t i = 0; i < vertices.length(); i++) {
        const auto &vertex = vertices[i];
        glVertex3f(vertex.getX(), vertex.getY(), vertex.getZ());
    }

    glEnd();
}

void Graphics::initializeGl() {
    if (glBuffer == nullptr) {
        glBuffer = ZB_open(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY(), ZB_MODE_RGBA,
            reinterpret_cast<void*>(bufferedLfb.getBuffer().get()));
        glInit(glBuffer);
    }

    const auto &scene = reinterpret_cast<D3::Scene&>(Game::getInstance().getCurrentScene());
    const auto width = static_cast<GLdouble>(bufferedLfb.getResolutionX());
    const auto height = static_cast<GLdouble>(bufferedLfb.getResolutionY());

    // Set clear color
    auto &clearColor = scene.getBackgroundColor();
    glClearColor(static_cast<float>(clearColor.getRed()) / 255,
        static_cast<float>(clearColor.getGreen()) / 255,
        static_cast<float>(clearColor.getBlue()) / 255, 1.0f);

    // Set up the view port to match the resolution of the frame buffer
    glViewport(0, 0, bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY());

    // Set the camera perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,             // Camera angle
                   width / height,   // Width to height ratio
                   1.0,              // Near z-clipping coordinate (Object nearer than that will not be drawn)
                   10000.0);    // Far z-clipping coordinate (Object farther than that will not be drawn)

    // Enable required OpenGL features
    glEnable(GL_DEPTH_TEST); // Depth testing to make sure the shapes are drawn in the correct order
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_NORMALIZE); // Automatically normalize normals

    // Set shade model and render style
    glShadeModel(scene.getGlShadeModel());
    glPolygonMode(GL_FRONT_AND_BACK, scene.getGlRenderStyle());

    glEnabled = true;
}

void Graphics::disableGl() {
    glEnabled = false;
}

bool Graphics::isGlEnabled() const {
    return glEnabled;
}

void Graphics::show() const {
    bufferedLfb.flush();

    if (glEnabled) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if (backgroundBuffer == nullptr) {
        // No background saved -> Start next frame with a clear screen
        bufferedLfb.clear();
    } else if (camera.getPosition().getX() == 0 && camera.getPosition().getY() == 0) {
        // A background is saved and the camera is at the origin -> Just copy the background
        const auto source = Util::Address(backgroundBuffer);
        bufferedLfb.getBuffer().copyRange(source, bufferedLfb.getResolutionY() * bufferedLfb.getPitch());
    } else {
        // A background is saved and the camera is not at the origin -> Scroll the background accordingly
        const auto pitch = bufferedLfb.getPitch();
        const auto colorDepthDivisor = (bufferedLfb.getColorDepth() == 15 ? 16 : bufferedLfb.getColorDepth()) / 8;
        auto xOffset = static_cast<size_t>(camera.getPosition().getX() * pitch / colorDepthDivisor) % pitch;
        xOffset -= xOffset % colorDepthDivisor;

        for (size_t i = 0; i < bufferedLfb.getResolutionY(); i++) {
            const auto yOffset = pitch * i;

            auto source = Util::Address(backgroundBuffer + yOffset + xOffset);
            auto target = bufferedLfb.getBuffer().add(yOffset);
            target.copyRange(source, pitch - xOffset);

            source = Util::Address(backgroundBuffer + yOffset);
            target = bufferedLfb.getBuffer().add(yOffset + (pitch - xOffset));
            target.copyRange(source, pitch - (pitch - xOffset));
        }
    }
}

void Graphics::setColor(const Util::Graphic::Color &color) {
    Graphics::color = color;
}

const Util::Graphic::Color& Graphics::getColor() const {
    return color;
}

void Graphics::saveCurrentStateAsBackground() {
    if (backgroundBuffer == nullptr) {
        backgroundBuffer = new uint8_t[bufferedLfb.getPitch() * bufferedLfb.getResolutionY()];
    }

    Util::Address(backgroundBuffer).copyRange(bufferedLfb.getBuffer(),
        bufferedLfb.getPitch() * bufferedLfb.getResolutionY());
}

void Graphics::clearBackground() {
    delete backgroundBuffer;
    backgroundBuffer = nullptr;
}

const Camera& Graphics::getCamera() const {
    return camera;
}

uint16_t Graphics::getAbsoluteResolutionX() const {
    return bufferedLfb.getResolutionX();
}

uint16_t Graphics::getAbsoluteResolutionY() const {
    return bufferedLfb.getResolutionY();
}

const Util::Math::Vector2<double>& Graphics::getDimensions() const {
    return dimensions;
}

uint16_t Graphics::getTransformation() const {
    return transformation;
}

double Graphics::getRelativeFontSize() const {
    return FONT_SIZE / static_cast<double>(transformation);
}

void Graphics::clear(const Util::Graphic::Color &color) const {
    if (glBuffer != nullptr) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    if (color == Util::Graphic::Colors::BLACK) {
        bufferedLfb.clear();
    } else {
        for (size_t i = 0; i < bufferedLfb.getResolutionX(); i++) {
            for (size_t j = 0; j < bufferedLfb.getResolutionY(); j++) {
                bufferedLfb.drawPixel(i, j, color);
            }
        }
    }
}

void Graphics::update() {
    camera = Game::getInstance().getCurrentScene().getCamera();

    if (isGlEnabled()) {
        // Set drawing perspective
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Set camera position and rotation
        glRotatef(camera.getRotation().getZ(),
            D3::Orientation::WORLD_UP.getX(),
            D3::Orientation::WORLD_UP.getY(),
            D3::Orientation::WORLD_UP.getZ()); // Yaw
        glRotatef(camera.getRotation().getY(),
            camera.getRightVector().getX(),
            camera.getRightVector().getY(),
            camera.getRightVector().getZ()); // Pitch
        glTranslatef(-camera.getPosition().getX(),
            -camera.getPosition().getY(),
            -camera.getPosition().getZ()); // Position

        // Lights
        const auto &scene = reinterpret_cast<D3::Scene&>(Game::getInstance().getCurrentScene());
        const auto &ambientLight = scene.getAmbientLight();
        GLfloat ambientColor[4] = {
            static_cast<float>(ambientLight.getRed()) / 255,
            static_cast<float>(ambientLight.getGreen()) / 255,
            static_cast<float>(ambientLight.getBlue()) / 255,
            1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

        for (const auto &light : scene.getLights()) {
            if (light.isValid()) {
                const auto &lightPosition = light.getPosition();
                const auto &diffuseLight = light.getDiffuseColor();
                const auto &specularLight = light.getSpecularColor();

                GLfloat position[4] = {
                    static_cast<GLfloat>(lightPosition.getX()),
                    static_cast<GLfloat>(lightPosition.getY()),
                    static_cast<GLfloat>(lightPosition.getZ()),
                    static_cast<GLfloat>(light.getType()) };
                GLfloat diffuse[4] = {
                    static_cast<GLfloat>(diffuseLight.getRed()) / 255,
                    static_cast<GLfloat>(diffuseLight.getGreen()) / 255,
                    static_cast<GLfloat>(diffuseLight.getBlue()) / 255, 1.0f
                };
                GLfloat specular[4] = {
                    static_cast<GLfloat>(specularLight.getRed()) / 255,
                    static_cast<GLfloat>(specularLight.getGreen()) / 255,
                    static_cast<GLfloat>(specularLight.getBlue()) / 255, 1.0f
                };

                const auto glLightEnum = static_cast<GLint>(GL_LIGHT0 + light.getIndex());
                glLightfv(glLightEnum, GL_POSITION, position);
                glLightfv(glLightEnum, GL_DIFFUSE, diffuse);
                glLightfv(glLightEnum, GL_SPECULAR, specular);
            }
        }
    }
}

void Graphics::drawImageDirect2D(const Util::Math::Vector2<double> &position, const Util::Graphic::Image &image,
    const bool flipX, const double alpha) const
{
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>(
        (position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(
        -(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

    if (xPixelOffset + image.getWidth() < 0 || xPixelOffset > bufferedLfb.getResolutionX() ||
        yPixelOffset - image.getHeight() > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    for (size_t i = 0; i < image.getHeight(); i++) {
        for (size_t j = 0; j < image.getWidth(); j++) {
            const auto &pixel = pixelBuffer[i * image.getWidth() + (flipX ? image.getWidth() - j : j)];
            bufferedLfb.drawPixel(xPixelOffset + j, yPixelOffset - i,
                pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageScaled2D(const Util::Math::Vector2<double> &position, const Util::Graphic::Image &image,
    const bool flipX, const double alpha, const Util::Math::Vector2<double> &scale) const
{
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int16_t>(
        (position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int16_t>(
        -(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

    const auto scaledWidth = static_cast<uint16_t>(image.getWidth() * scale.getX());
    const auto scaledHeight = static_cast<uint16_t>(image.getHeight() * scale.getY());

    if (xPixelOffset + scaledWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() ||
        yPixelOffset - scaledHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    const auto factorX = static_cast<double>(scaledWidth) / image.getWidth();
    const auto factorY = static_cast<double>(scaledHeight) / image.getHeight();

    for (size_t i = 0; i < scaledHeight; i++) {
        for (size_t j = 0; j < scaledWidth; j++) {
            const auto imageX = static_cast<uint16_t>(j / factorX);
            const auto imageY = static_cast<uint16_t>(i / factorY);

            const auto &pixel = pixelBuffer[imageY * image.getWidth() +
                (flipX ? image.getWidth() - imageX : imageX)];
            bufferedLfb.drawPixel(xPixelOffset + j, yPixelOffset - i,
                pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageRotated2D(const Util::Math::Vector2<double> &position, const Util::Graphic::Image &image,
    const bool flipX, const double alpha, const double rotationAngle) const
{
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>(
        (position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(
        -(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

    const auto centerX = image.getWidth() / 2.0;
    const auto centerY = image.getHeight() / 2.0;
    const auto rotatedHeight = static_cast<uint16_t>(image.getWidth() *
        Util::Math::absolute(Util::Math::sine(rotationAngle)) + image.getHeight() *
        Util::Math::absolute(Util::Math::cosine(rotationAngle)));
    const auto rotatedWidth = static_cast<uint16_t>(image.getWidth() *
        Util::Math::absolute(Util::Math::cosine(rotationAngle)) + image.getHeight() *
        Util::Math::absolute(Util::Math::sine(rotationAngle)));

    if (xPixelOffset + rotatedWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() ||
        yPixelOffset - rotatedHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    const auto yOffset = rotatedHeight - image.getHeight();
    const auto xOffset = rotatedWidth - image.getWidth();

    for (int32_t i = -yOffset; i < rotatedHeight; i++) {
        for (int32_t j = -xOffset; j < rotatedWidth; j++) {
            const auto imageX = static_cast<uint16_t>(centerX + (j - centerX) * Util::Math::cosine(rotationAngle) +
                (i - centerY) * Util::Math::sine(rotationAngle));
            const auto imageY = static_cast<uint16_t>(centerY - (j - centerX) * Util::Math::sine(rotationAngle) +
                (i - centerY) * Util::Math::cosine(rotationAngle));

            if (imageX >= image.getWidth() || imageY >= image.getHeight()) {
                continue;
            }

            const auto &pixel = pixelBuffer[imageY * image.getWidth() +
                (flipX ? image.getWidth() - imageX : imageX)];
            bufferedLfb.drawPixel(xPixelOffset + j, yPixelOffset - i,
                pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }
}

void Graphics::drawImageScaledAndRotated2D(const Util::Math::Vector2<double> &position,
    const Util::Graphic::Image &image, const bool flipX, const double alpha, const Util::Math::Vector2<double> &scale,
    const double rotationAngle) const
{
    const auto *pixelBuffer = image.getPixelBuffer();
    const auto xPixelOffset = static_cast<int32_t>(
        (position.getX() - camera.getPosition().getX()) * transformation + offsetX);
    const auto yPixelOffset = static_cast<int32_t>(
        -(position.getY() - camera.getPosition().getY()) * transformation + offsetY);

    const auto scaledWidth = static_cast<uint16_t>(image.getWidth() * scale.getX());
    const auto scaledHeight = static_cast<uint16_t>(image.getHeight() * scale.getY());

    const auto centerX = scaledWidth / 2.0;
    const auto centerY = scaledHeight / 2.0;
    const auto rotatedHeight = static_cast<uint16_t>(scaledWidth *
        Util::Math::absolute(Util::Math::sine(rotationAngle)) + scaledHeight *
        Util::Math::absolute(Util::Math::cosine(rotationAngle)));
    const auto rotatedWidth = static_cast<uint16_t>(scaledWidth *
        Util::Math::absolute(Util::Math::cosine(rotationAngle)) + scaledHeight *
        Util::Math::absolute(Util::Math::sine(rotationAngle)));

    if (xPixelOffset + rotatedWidth < 0 || xPixelOffset > bufferedLfb.getResolutionX() ||
        yPixelOffset - rotatedHeight > bufferedLfb.getResolutionY() || yPixelOffset < 0) {
        return;
    }

    // Scale image into new buffer
    auto *scaledPixelBuffer = new Util::Graphic::Color[scaledWidth * scaledHeight];
    const auto factorX = static_cast<double>(scaledWidth) / image.getWidth();
    const auto factorY = static_cast<double>(scaledHeight) / image.getHeight();

    for (int i = 0; i < scaledHeight; i++) {
        for (int j = 0; j < scaledWidth; j++) {
            const auto imageX = static_cast<uint16_t>(j / factorX);
            const auto imageY = static_cast<uint16_t>(i / factorY);
            scaledPixelBuffer[scaledWidth * i + j] = pixelBuffer[image.getWidth() * imageY + imageX];
        }
    }

    // Draw rotated image from scaled buffer
    const auto yOffset = rotatedHeight - scaledHeight;
    const auto xOffset = rotatedWidth - scaledWidth;

    for (int32_t i = -yOffset; i < scaledHeight + yOffset; i++) {
        for (int32_t j = -xOffset; j < scaledWidth + xOffset; j++) {
            const auto imageX = static_cast<uint16_t>(centerX + (j - centerX) * Util::Math::cosine(rotationAngle) +
                (i - centerY) * Util::Math::sine(rotationAngle));
            const auto imageY = static_cast<uint16_t>(centerY - (j - centerX) * Util::Math::sine(rotationAngle) +
                (i - centerY) * Util::Math::cosine(rotationAngle));

            if (imageX >= scaledWidth || imageY >= scaledHeight) {
                continue;
            }

            const auto &pixel = scaledPixelBuffer[imageY * scaledWidth +
                (flipX ? scaledWidth - imageX : imageX)];
            bufferedLfb.drawPixel(xPixelOffset + j, yPixelOffset - i,
                pixel.withAlpha(static_cast<uint8_t>(pixel.getAlpha() * alpha)));
        }
    }

    delete[] scaledPixelBuffer;
}

void Graphics::gluPerspective(const GLdouble fovY, const GLdouble aspect, const GLdouble zNear, const GLdouble zFar) {
    const GLdouble fH = Util::Math::tangent(fovY / 360 * Util::Math::PI_FLOAT) * zNear;
    const GLdouble fW = fH * aspect;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void Graphics::gluMultOrthoMatrix(const float left, const float right, const float bottom, const float top,
    const float near, const float far)
{
    const float tx= -((right + left) / (right - left));
    const float ty= -((top + bottom) / (top - bottom));
    const float tz= -((far + near) / (far - near));

    const GLfloat ortho[16] = {
        2.0f / (right - left), 0.0f, 0.0f, tx,
        0.0f, 2.0f / (top - bottom), 0.0f, ty,
        0.0f, 0.0f, -2.0f / (far - near), tz,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMultMatrixf(ortho);
}

void Graphics::gluPrepareDirectDraw(const GLint renderStyle) const {
    const auto &scene = reinterpret_cast<D3::Scene&>(Game::getInstance().getCurrentScene());

    glPolygonMode(GL_FRONT_AND_BACK, renderStyle);
    glDisable(GL_DEPTH_TEST);
    if (scene.isLightingEnabled()) {
        glDisable(GL_LIGHTING);
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Ortho matrix for 2D element
    gluMultOrthoMatrix(
        -dimensions.getX(), dimensions.getX(),
        -dimensions.getY(), dimensions.getY(),
        -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void Graphics::gluFinishDirectDraw() const {
    const auto &scene = reinterpret_cast<D3::Scene&>(Game::getInstance().getCurrentScene());

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    if (scene.isLightingEnabled()) {
        glEnable(GL_LIGHTING);
    }
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, scene.getGlRenderStyle());
}

}

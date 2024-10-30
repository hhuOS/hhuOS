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
 * Based on https://videotutorialsrock.com/opengl_tutorial/cube/home.php
 */

#include <stdint.h>
#include <GL/gl.h>

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/math/Random.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"

const constexpr uint32_t TARGET_FRAME_RATE = 60;

static Util::Time::Timestamp frameTime;
static Util::Time::Timestamp fpsTimer;
static uint32_t fpsCounter = 0;
static uint32_t fps = 0;

static Util::Math::Random random;

struct Cube {
    GLfloat size = random.nextRandomNumber() * 10 + 1;
    GLfloat coordinates[3] = { static_cast<GLfloat>(random.nextRandomNumber() * 50 - 25), static_cast<GLfloat>(random.nextRandomNumber() * 50 - 25), -(static_cast<GLfloat>(random.nextRandomNumber() * 100 + 20)) };

    GLfloat rotationAngle = 0;
    GLfloat rotationSpeed = random.nextRandomNumber() * 100 + 10;
    GLfloat rotationAxes[3] = {static_cast<GLfloat>(random.nextRandomNumber() * 200) - 100, static_cast<GLfloat>(random.nextRandomNumber() * 200) - 100, static_cast<GLfloat>(random.nextRandomNumber() * 200) - 100 };

    GLuint texture = 0;
    GLuint displayList = 0;

    Cube() = default;

    explicit Cube(GLuint texture) : texture(texture) {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        //Top face
        glNormal3f(0.0, 1.0f, 0.0f);
        glVertex3f(-size / 2, size / 2, -size / 2);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size / 2, size / 2, size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size / 2, size / 2, size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size / 2, size / 2, -size / 2);
        glTexCoord2f(0.0f, 1.0f);

        //Bottom face
        glNormal3f(0.0, -1.0f, 0.0f);
        glVertex3f(-size / 2, -size / 2, -size / 2);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(size / 2, -size / 2, -size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size / 2, -size / 2, size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-size / 2, -size / 2, size / 2);
        glTexCoord2f(0.0f, 1.0f);

        //Left face
        glNormal3f(-1.0, 0.0f, 0.0f);
        glVertex3f(-size / 2, -size / 2, -size / 2);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size / 2, -size / 2, size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-size / 2, size / 2, size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-size / 2, size / 2, -size / 2);
        glTexCoord2f(0.0f, 1.0f);

        //Right face
        glNormal3f(1.0, 0.0f, 0.0f);
        glVertex3f(size / 2, -size / 2, -size / 2);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(size / 2, size / 2, -size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size / 2, size / 2, size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size / 2, -size / 2, size / 2);
        glTexCoord2f(0.0f, .0f);

        glEnd();

        glBegin(GL_QUADS);

        //Front face
        glNormal3f(0.0, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size / 2, -size / 2, size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size / 2, -size / 2, size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size / 2, size / 2, size / 2);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-size / 2, size / 2, size / 2);

        //Back face
        glNormal3f(0.0, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size / 2, -size / 2, -size / 2);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-size / 2, size / 2, -size / 2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size / 2, size / 2, -size / 2);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(size / 2, -size / 2, -size / 2);

        glEnd();
        glDisable(GL_TEXTURE_2D);

        glEndList();
    }

    bool operator!=(const Cube &other) {
        return displayList != other.displayList;
    }

    void update(const Util::Time::Timestamp &frameTime) {
        rotationAngle += (static_cast<GLfloat>(frameTime.toMicroseconds()) / 1000000) * rotationSpeed;
    }
    
    void draw() const {
        glLoadIdentity();
        glTranslatef(coordinates[0], coordinates[1], coordinates[2]);
        glRotatef(rotationAngle, rotationAxes[0], rotationAxes[1], rotationAxes[2]);
        glCallList(displayList);
    }
};

GLuint loadTexture(Util::Graphic::Image *image) {
    auto *rawImageData = new uint8_t[image->getWidth() * image->getHeight() * 3];
    for (uint32_t i = 0; i < image->getWidth() * image->getHeight(); i++) {
        auto color = image->getPixelBuffer()[i];
        rawImageData[i * 3] = color.getRed();
        rawImageData[i * 3 + 1] = color.getGreen();
        rawImageData[i * 3 + 2] = color.getBlue();
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->getWidth(), image->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, rawImageData);

    delete[] rawImageData;
    return textureId;
}

void cubes(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    auto &font = Util::Graphic::Font::getFontForResolution(lfb.getResolutionY());

    // Initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat h = (GLfloat) lfb.getResolutionY() / (GLfloat) lfb.getResolutionX();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -h, h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);

    // Load texture
    auto *bitmap = Util::Graphic::BitmapFile::open("/user/dino/block/box.bmp");
    auto texture = loadTexture(bitmap);
    delete bitmap;

    auto *cubes = new Util::ArrayList<Cube*>();
    for (uint32_t i = 0; i < 5; i++) {
        cubes->add(new Cube(texture));
    }

    auto keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());

    // Draw scene
    while (true) {
        auto startTime = Util::Time::getSystemTime();

        auto c = Util::System::in.read();
        if (c > 0 && keyDecoder->parseScancode(c)) {
            auto key = keyDecoder->getCurrentKey();
            if (key.isPressed()) {
                if (key.getScancode() == Util::Io::Key::ESC) {
                    break;
                } else if (key.getScancode() == Util::Io::Key::PLUS) {
                    cubes->add(new Cube(texture));
                } else if (key.getScancode() == Util::Io::Key::MINUS) {
                    if (cubes->size() > 0) {
                        delete cubes->pop();
                    }
                }
            }
        }

        // Rotate cubes
        for (auto *cube : *cubes) {
            cube->update(frameTime);
        }

        // Draw the scene
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

        GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
        GLfloat lightPos[] = {-20, 10, 40, 1.0f};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        for (const auto *cube : *cubes) {
            cube->draw();
        }

        // Draw the FPS string on top of the rendered OpenGL scene
        stringDrawer.drawString(font, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        stringDrawer.drawString(font, 0, font.getCharHeight(), static_cast<const char*>(Util::String::format("Cubes: %u", cubes->size())), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        lfb.flush(); // Flushes the buffered frame buffer to the screen

        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        fpsCounter++;
        frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }
    }

    while (!cubes->isEmpty()) {
        delete cubes->pop();
    }
}
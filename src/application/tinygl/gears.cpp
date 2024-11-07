/*
 * Based on the TinyGL gears demo (https://github.com/C-Chads/tinygl/blob/main/Raw_Demos/gears.c)
 */

#include <math.h>
#include <stdint.h>
#include <GL/gl.h>

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/math/Math.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"

const constexpr uint32_t TARGET_FRAME_RATE = 60;
const constexpr GLfloat VIEW_ROTATION_X = 20.0;
const constexpr GLfloat VIEW_ROTATION_Y = 30.0;
const constexpr GLfloat PI = Util::Math::PI;

static Util::Time::Timestamp fpsTimer;
static uint32_t fpsCounter = 0;
static uint32_t fps = 0;

static GLfloat rotationAngle = 0;
static GLuint gear1, gear2, gear3;

static void gear(GLfloat innerRadius, GLfloat outerRadius, GLfloat width, GLint teeth, GLfloat toothDepth) {
    GLint i;
    GLfloat r0, r1, r2;
    GLfloat angle, da;
    GLfloat u, v, len;

    r0 = innerRadius;
    r1 = outerRadius - toothDepth / 2.0f;
    r2 = outerRadius + toothDepth / 2.0f;

    da = 2.0 * PI / teeth / 4.0;

    glNormal3f(0.0, 0.0, 1.0);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * PI / teeth;
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
    glEnd();

    glBegin(GL_QUADS);
    da = 2.0 * PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * PI / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * PI / teeth;
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
    glEnd();

    glBegin(GL_QUADS);
    da = 2.0 * PI / teeth / 4.0;
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * PI / teeth;

        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < teeth; i++) {
        angle = i * 2.0 * PI / teeth;

        glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
        glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
        u = r2 * cos(angle + da) - r1 * cos(angle);
        v = r2 * sin(angle + da) - r1 * sin(angle);
        len = sqrt(u * u + v * v);
        u /= len;
        v /= len;
        glNormal3f(v, -u, 0.0);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
        glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
        glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
        u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
        v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
        glNormal3f(v, -u, 0.0);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
        glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
        glNormal3f(cos(angle), sin(angle), 0.0);
    }

    glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
    glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = i * 2.0 * PI / teeth;
        glNormal3f(-cos(angle), -sin(angle), 0.0);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
        glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    }
    glEnd();
}

void drawGears() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(VIEW_ROTATION_X, 1.0, 0.0, 0.0);
    glRotatef(VIEW_ROTATION_Y, 0.0, 1.0, 0.0);

    glPushMatrix();
    glTranslatef(-3.0, -2.0, 0.0);
    glRotatef(rotationAngle, 0.0, 0.0, 1.0);
    glCallList(gear1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.1, -2.0, 0.0);
    glRotatef(-2.0f * rotationAngle - 9.0f, 0.0, 0.0, 1.0);
    glCallList(gear2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.1, 4.2, 0.0);
    glRotatef(-2.0f * rotationAngle - 25.0f, 0.0, 0.0, 1.0);
    glCallList(gear3);
    glPopMatrix();

    glPopMatrix();
}

void gears(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    auto &font = Util::Graphic::Font::getFontForResolution(lfb.getResolutionY());

    // Initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    glEnable(GL_LIGHTING);
    glBlendEquation(GL_FUNC_ADD);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    GLfloat h = (GLfloat) lfb.getResolutionY() / (GLfloat) lfb.getResolutionX();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -45.0);

    static GLfloat pos[4] = {5, 5, 10, 0.0}; // Light at infinity.

    static GLfloat red[4] = {1.0, 0.0, 0.0, 0.0};
    static GLfloat green[4] = {0.0, 1.0, 0.0, 0.0};
    static GLfloat blue[4] = {0.0, 0.0, 1.0, 0.0};
    static GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};
    static GLfloat shininess = 5;
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHT0);

    // Create the gears
    gear1 = glGenLists(1);
    glNewList(gear1, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glColor3fv(blue);
    gear(1.0, 4.0, 1.0, 20, 0.7); // The largest gear
    glEndList();

    gear2 = glGenLists(1);
    glNewList(gear2, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor3fv(red);
    gear(0.5, 2.0, 2.0, 10, 0.7); // The small gear with the smaller hole, to the right
    glEndList();

    gear3 = glGenLists(1);
    glNewList(gear3, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor3fv(green);
    gear(1.3, 2.0, 0.5, 10, 0.7); // The small gear above with the large hole
    glEndList();

    // Draw scene
    while (true) {
        auto startTime = Util::Time::getSystemTime();

        if (Util::System::in.read() > 0) {
            break;
        }

        // Draw the gears
        drawGears();

        // Draw the FPS string on top of the rendered OpenGL scene
        stringDrawer.drawString(font, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        lfb.flush(); // Flushes the buffered frame buffer to the screen

        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        fpsCounter++;
        auto frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }

        rotationAngle += (static_cast<GLfloat>(frameTime.toMicroseconds()) / 1000000) * 75;
    }
}
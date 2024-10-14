/*
 * Lesson 7: Terrain (https://videotutorialsrock.com/opengl_tutorial/terrain/home.php)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Ported to hhuOS by Fabian Ruhland, 14.10.2024
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
#include "lib/util/io/key/Key.h"
#include "lib/util/async/Process.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "application/tinygl/util.h"
#include "lib/util/math/Vector3D.h"
#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/math/Math.h"

class Terrain {

public:
    /**
     * Load a terrain from a heightmap (bitmap file). The heights of the terrain range from (-height / 2) to (height / 2).
     */
    static Terrain* loadTerrain(const char *path, float height) {
        auto *image = Util::Graphic::BitmapFile::open(path);
        auto *pixels = image->getPixelBuffer();
        auto *terrain = new Terrain(image->getWidth(), image->getHeight());

        for (int y = 0; y < image->getHeight(); y++) {
            for (int x = 0; x < image->getWidth(); x++) {
                auto color = pixels[y * image->getWidth() + x]; // Get the color of the pixel at (x, y)
                float z = height * ((color.getRed() / 255.0f) - 0.5f); // Image is black & white, so all color channels are the same
                terrain->setHeight(x, y, z); // Set the height of the terrain at (x, y) to z
            }
        }

        delete image;
        terrain->computeNormals();

        return terrain;
    }

    /**
     * Destructor.
     * Frees the allocated memory for height and normal arrays.
     */
    ~Terrain() {
        for (uint32_t i = 0; i < length; i++) {
            delete[] heights[i];
        }
        delete[] heights;

        for (uint32_t i = 0; i < length; i++) {
            delete[] normals[i];
        }
        delete[] normals;
    }

    /**
     * Get the width of the terrain.
     */
    [[nodiscard]] uint32_t getWidth() const {
        return width;
    }

    /**
     * Get the length of the terrain.
     */
    [[nodiscard]] uint32_t getLength() const {
        return length;
    }

    /**
     * Set the height of the point (x, z) in the terrain to y.
     */
    void setHeight(uint32_t x, uint32_t z, float y) {
        heights[z][x] = y;
        computedNormals = false;
    }

    /**
     * Get the height of the point (x, z) in the terrain.
     */
    [[nodiscard]] float getHeight(uint32_t x, uint32_t z) const {
        return heights[z][x];
    }

    /**
     * Compute normals, if they are not already computed.
     */
    void computeNormals() {
        if (computedNormals) {
            return;
        }

        auto **normals2 = new Util::Math::Vector3D*[length];
        for (uint32_t i = 0; i < length; i++) {
            normals2[i] = new Util::Math::Vector3D[width];
        }

        for (uint32_t z = 0; z < length; z++) {
            for (uint32_t x = 0; x < width; x++) {
                Util::Math::Vector3D sum(0.0f, 0.0f, 0.0f);

                Util::Math::Vector3D out;
                if (z > 0) {
                    out = Util::Math::Vector3D(0.0f, heights[z - 1][x] - heights[z][x], -1.0f);
                }

                Util::Math::Vector3D in;
                if (z < length - 1) {
                    in = Util::Math::Vector3D(0.0f, heights[z + 1][x] - heights[z][x], 1.0f);
                }

                Util::Math::Vector3D left;
                if (x > 0) {
                    left = Util::Math::Vector3D(-1.0f, heights[z][x - 1] - heights[z][x], 0.0f);
                }

                Util::Math::Vector3D right;
                if (x < width - 1) {
                    right = Util::Math::Vector3D(1.0f, heights[z][x + 1] - heights[z][x], 0.0f);
                }

                if (x > 0 && z > 0) {
                    sum = sum + out.cross(left).normalize();
                }

                if (x > 0 && z < length - 1) {
                    sum = sum + left.cross(in).normalize();
                }

                if (x < width - 1 && z < length - 1) {
                    sum = sum + in.cross(right).normalize();
                }

                if (x < width - 1 && z > 0) {
                    sum = sum + right.cross(out).normalize();
                }

                normals2[z][x] = sum;
            }
        }

        const float FALLOUT_RATIO = 0.5f;
        for (uint32_t z = 0; z < length; z++) {
            for (uint32_t x = 0; x < width; x++) {
                auto sum = normals2[z][x];

                if (x > 0) {
                    sum = sum + normals2[z][x - 1] * FALLOUT_RATIO;
                }
                if (x < width - 1) {
                    sum = sum + normals2[z][x + 1] * FALLOUT_RATIO;
                }
                if (z > 0) {
                    sum = sum + normals2[z - 1][x] * FALLOUT_RATIO;
                }
                if (z < length - 1) {
                    sum = sum + normals2[z + 1][x] * FALLOUT_RATIO;
                }

                if (sum.length() == 0) {
                    sum = Util::Math::Vector3D(0.0f, 1.0f, 0.0f);
                }
                normals[z][x] = sum;
            }
        }

        for (uint32_t i = 0; i < length; i++) {
            delete[] normals2[i];
        }
        delete[] normals2;

        computedNormals = true;
    }

    /**
     * Get the normal at the point (x, z) in the terrain.
     */
    [[nodiscard]] Util::Math::Vector3D getNormal(uint32_t x, uint32_t z) {
        if (!computedNormals) {
            computeNormals();
        }

        return normals[z][x];
    }

private:
    /**
     * Constructor.
     * Allocates memory for height and normal arrays.
     */
    Terrain(uint32_t width, uint32_t length) : width(width), length(length) {
        heights = new float*[length];
        for (uint32_t i = 0; i < length; i++) {
            heights[i] = new float[width];
        }

        normals = new Util::Math::Vector3D*[length];
        for (uint32_t i = 0; i < length; i++) {
            normals[i] = new Util::Math::Vector3D[width];
        }

        computedNormals = false;
    }

    uint32_t width;
    uint32_t length;
    float **heights;
    Util::Math::Vector3D **normals;
    bool computedNormals;
};

/**
 * The target frame rate of the application.
 * If the application achieves this frame rate, it will sleep for the remaining time of the frame.
 */
const constexpr uint32_t TARGET_FRAME_RATE = 60;

/**
 * The current rotation angle of the terrain.
 * This angle is increased continuously to rotate the terrain around the y-axis.
 */
static GLfloat rotationAngle = 60;

/**
 * The terrain that will be drawn.
 */
static Terrain *terrain = nullptr;

/**
 * Called when the user presses ESC to exit the application.
 * Frees all allocated resources.
 */
void cleanup() {
    delete terrain;
}

/**
 * Called before the main loop to initialize the OpenGL scene.
 */
static void initRendering(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto width = static_cast<GLdouble>(lfb.getResolutionX());
    const auto height = static_cast<GLdouble>(lfb.getResolutionY());

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up the view port to match the resolution of the frame buffer
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    // Set the camera perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,             // Camera angle
                   width / height,   // Width to height ratio
                   1.0,              // Near z-clipping coordinate (Object nearer than that will not be drawn)
                   200.0);           // Far z-clipping coordinate (Object farther than that will not be drawn)

    // Enable required OpenGL features
    glEnable(GL_DEPTH_TEST); // Depth testing to make sure the shapes are drawn in the correct order
    glEnable(GL_COLOR_MATERIAL); // Enable color
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_LIGHT0); // Enable light #0
    glEnable(GL_NORMALIZE); // Automatically normalize normals
}

/**
 * Called by the main loop before updating the scene, when a key is pressed.
 */
static void handleKeypress(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            cleanup();
            Util::Async::Process::exit(0);
    }
}

/**
 * Called by the main loop before drawing the scene to apply updates to all objects.
 */
static void update(const Util::Time::Timestamp &time) {
    // Rotate the cube
    rotationAngle += (static_cast<GLfloat>(time.toMicroseconds()) / 1000000) * 40;
    if (rotationAngle > 360) {
        rotationAngle -= 360;
    }
}

/**
 * Called in each iteration of the main loop to draw the scene.
 */
static void drawScene() {
    // Clear information from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); // Switch to drawing perspective
    glLoadIdentity(); // Reset the drawing perspective

    // Set translation and rotation for the terrain
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(30, 1, 0, 0);
    glRotatef(rotationAngle, 0, 1, 0);

    // Add ambient light
    GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Add positioned light
    GLfloat lightColor[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat lightPosition[] = { -0.f, 0.8f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    float scale = 5.0f / Util::Math::max(terrain->getWidth() - 1.0f, terrain->getLength() - 1.0f);
    glScalef(scale, scale, scale);
    glTranslatef(-(terrain->getWidth() - 1.0f) / 2, 0.0f, -(terrain->getLength() - 1.0f) / 2);

    glColor3f(0.3f, 0.9f, 0.0f);
    for (uint32_t z = 0; z < terrain->getLength() - 1; z++) {
        // Makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for (uint32_t x = 0; x < terrain->getWidth(); x++) {
            auto normal = terrain->getNormal(x, z);
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
            glVertex3f(x, terrain->getHeight(x, z), z);
            normal = terrain->getNormal(x, z + 1);
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
            glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
        }
        glEnd();
    }
}

/**
 * Lesson 7: Terrain (https://videotutorialsrock.com/opengl_tutorial/terrain/home.php)
 */
void lesson7(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    auto &font = Util::Graphic::Font::getFontForResolution(lfb.getResolutionY());

    // Variable needed for FPS calculation
    static Util::Time::Timestamp frameTime;
    Util::Time::Timestamp fpsTimer;
    uint32_t fpsCounter = 0;
    uint32_t fps = 0;

    // Needed to parse key presses
    auto keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());

    // Load terrain
    terrain = Terrain::loadTerrain("/user/demo/heightmap.bmp", 20);

    // Initialize GL
    initRendering(lfb);

    // Draw scene
    while (true) {
        // Get current time at the start of drawing the current frame (needed for FPS calculation)
        auto startTime = Util::Time::getSystemTime();

        // Check for key presses
        auto c = Util::System::in.read();
        if (c > 0 && keyDecoder->parseScancode(c)) {
            auto key = keyDecoder->getCurrentKey();
            if (key.isPressed()) {
                handleKeypress(key);
            }
        }

        // Update the scene
        update(frameTime);

        // Draw the scene
        drawScene();

        // Draw the FPS string on top of the rendered OpenGL scene
        stringDrawer.drawString(font, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        lfb.flush(); // Flushes the buffered frame buffer to the screen

        // Calculate the time the rendering took and sleep if necessary to achieve the target frame rate
        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        // Calculate the FPS
        fpsCounter++;
        frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }
    }
}
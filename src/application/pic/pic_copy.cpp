#include <cstdint>

#include "lib/util/base/System.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/base/ArgumentParser.h"
#include <cstdint>
#include <device/cpu/Cpu.h>
#include <kernel/service/ProcessService.h>
#include <lib/util/base/System.h>
#include <lib/util/async/Runnable.h>
#include <lib/util/async/Thread.h>
#include <lib/util/math/Random.h>
#include <lib/util/time/Timestamp.h>
#include "lib/util/async/Process.h"


#include "stb_image.h"
#include "stb_image_write.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "lib/util/graphic/font/Sun8x16.h"
#include "lib/libc/math.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

using namespace Util::Graphic;

void first(uint32_t *buffer, int x, int y, int pitch);

void second(uint32_t *buffer, int x, int y, int pitch, StringDrawer &stringDrawer);

void third(uint32_t *buffer, int pitch);

void fourth(uint32_t *buffer, int pitch);

void fifth(uint32_t *buffer, int pitch, StringDrawer &stringDrawer);

void calcPicData(int *picData, const unsigned char *data);

BufferedLinearFrameBuffer *blfb;
Util::Io::FileInputStream *mouseInputStream;
Util::Io::KeyDecoder keyDecoder = Util::Io::KeyDecoder(new Util::Io::DeLayout());
int mouseY = 0;
int mouseX = 0;
bool leftButtonPressed = false;
bool running = true;
int posX = 0, posY = 0, rotationDegree = 0, scale = 1;
int width, height, channels;
bool changed = true, save = false;

class RandomCalculationRunnable : public Util::Async::Runnable {
public:
    explicit RandomCalculationRunnable() = default;

    void run() override {
        Util::Math::Random random = Util::Math::Random();
        int counter = 0;
        while (true) {
            // Perform some random calculations
            double a = random.nextRandomNumber();
            double b = random.nextRandomNumber();
            double result = a * b + (a - b);

            // This is where you'd put your breakpoint
            Util::System::out << counter << " - Random calculation result: " << result << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            counter++;

            // Sleep for 1 second
            Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(5));
        }
    }
};

int32_t main_(int32_t argc, char *argv[]) {

//    auto dings = Util::Async::Thread::createThread("randomStuff", new RandomCalculationRunnable());

    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Fick dich doch\n");
    argumentParser.addArgument("number", false, "n");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl
                            << Util::Io::PrintStream::flush;
        return -1;
    }

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = LinearFrameBuffer(lfbFile);
    blfb = new BufferedLinearFrameBuffer(lfb, true);

    Ansi::prepareGraphicalApplication(true);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    print("blfb-ColorDepth: " << blfb->getColorDepth());
    print("blfb-ResX: " << blfb->getResolutionX());
    print("blfb-ResY: " << blfb->getResolutionY());
    print("blfb-Pitch: " << blfb->getPitch());
    PixelDrawer drawer(*blfb);
    StringDrawer stringDrawer(drawer);

    auto *buffer = reinterpret_cast<uint32_t *>(blfb->getBuffer().get());
    int x = blfb->getResolutionX(), y = blfb->getResolutionY(), pitch = blfb->getPitch();
    blfb->clear();

    auto mouseFile = Util::Io::File("/device/mouse");
    mouseInputStream = new Util::Io::FileInputStream(mouseFile);
    mouseInputStream->setAccessMode(Util::Io::File::NON_BLOCKING);

    auto processDirectory = Util::Io::File("/process");
    auto processRootPath = processDirectory.getCanonicalPath();

    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW << "PID\tThreads\tName" << Util::Graphic::Ansi::FOREGROUND_DEFAULT << Util::Io::PrintStream::endl;
    for (const auto &child : processDirectory.getChildren()) {
        auto processPath = processRootPath + "/" + child + "/";

        auto nameFile = Util::Io::File(processPath + "/name");
        auto nameStream = Util::Io::FileInputStream(nameFile);

        auto threadCountFile = Util::Io::File(processPath + "/thread_count");
        auto threadCountStream = Util::Io::FileInputStream(threadCountFile);

        Util::System::out << child << "\t"
                          << threadCountStream.readString(threadCountFile.getLength() - 1) << "\t"
                          << nameStream.readString(nameFile.getLength() - 1) << Util::Io::PrintStream::endl;
    }

    Util::System::out << Util::Io::PrintStream::flush;

    if (argumentParser.hasArgument("number")) {
        auto arg = argumentParser.getArgument("number")[0];
        switch (arg) {
            case '1':
                first(buffer, x, y, pitch);
                break;
            case '2':
                second(buffer, x, y, pitch, stringDrawer);
                break;
            case '3':
                third(buffer, pitch);
                break;
            case '4':
                fourth(buffer, pitch);
                break;
            default:
                print("Invalid argument: " << arg);
                break;
        }
    } else {
        //    first(buffer, x, y, pitch);
        //    second(buffer, x, y, pitch, stringDrawer);
                third(buffer, pitch);
//        fifth(buffer, pitch, stringDrawer);
    }


    Ansi::cleanupGraphicalApplication();

    return 0;
}

void checkMouseInput() {
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = mouseInputStream->read();
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            auto min = [](int a, int b) { return a < b ? a : b; };
            auto max = [](int a, int b) { return a > b ? a : b; };
            mouseX = max(0, min(blfb->getResolutionX() - 1, mouseX + mouseUpdate.xMovement));
            mouseY = max(0, min(blfb->getResolutionY() - 1, mouseY - mouseUpdate.yMovement));
            leftButtonPressed = mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON;
            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
        }
        value = mouseInputStream->read();
    }
}

void checkKeyboardInput() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();
            switch (key.getScancode()) {
                case Util::Io::Key::ESC:
                    running = false;
                    break;
                case Util::Io::Key::UP:
                    posY -= 10;
                    break;
                case Util::Io::Key::DOWN:
                    posY += 10;
                    break;
                case Util::Io::Key::LEFT:
                    posX -= 10;
                    break;
                case Util::Io::Key::RIGHT:
                    posX += 10;
                    break;
                case Util::Io::Key::D:
                    rotationDegree += 10;
                    break;
                case Util::Io::Key::A:
                    rotationDegree -= 10;
                    break;
                case Util::Io::Key::W:
                    scale += 1;
                    break;
                case Util::Io::Key::S:
                    scale -= 1;
                    break;
                case Util::Io::Key::SPACE:
                    save = true;
                    break;
            }
            changed = true;
        }
        scancode = Util::System::in.read();
    }
}

void first(uint32_t *buffer, int x, int y, int pitch) {
    bool red = true, blue = true, green = false;
    auto time = Util::Time::getSystemTime();
    for (;;) {
        if (Util::System::in.read() > 0) {
            break;
        }
        for (int j = 0; j < x; j++) {
            for (int k = 0; k < y; k++) {
                uint8_t r = 0, g = 0, b = 0;
                if (red) {
                    r = static_cast<uint8_t>(j * 255 / x);
                }
                if (green) {
                    g = static_cast<uint8_t>(k * 255 / y);
                }
                if (blue) {
                    b = static_cast<uint8_t>((j + k) * 255 / (x + y));
                }
                const auto offset = j + k * (pitch / 4);
                uint32_t rgbColor = (0xff << 24) + (r << 16) + (g << 8) + b;
                reinterpret_cast<uint32_t *>(buffer)[offset] = rgbColor;
            }
        }
        auto now = Util::Time::getSystemTime();
        if (now - time > Util::Time::Timestamp::ofMilliseconds(1000)) {
            time = now;
            if (red && blue && !green) {
                red = false;
                blue = true;
                green = true;
            } else if (!red && blue && green) {
                red = true;
                blue = false;
                green = true;
            } else if (red && !blue && green) {
                red = true;
                blue = true;
                green = false;
            }
        }
    }
}

void second(uint32_t *buffer, int x, int y, int pitch, StringDrawer &stringDrawer) {
    uint8_t r = 255, g = 0, b = 0;
    int totalPixels = x * y;
    int state = 1;
    for (;;) {
        if (Util::System::in.read() > 0) {
            break;
        }
        for (int i = 0; i < totalPixels; i++) {
            uint32_t rgbColor = (0xff << 24) + (r << 16) + (g << 8) + b;
            buffer[i] = rgbColor;
        }
        stringDrawer.drawChar(Fonts::TERMINAL_8x8, 10, 10, static_cast<char>(state + '0'), Color(0, 0, 0),
                              Color(255, 255, 255));
        switch (state) {
            case 1:
                if (++g == 255) state = 2;
                break; // gr
            case 2:
                if (--r == 0) state = 3;
                break;  // g
            case 3:
                if (++b == 255) state = 4;
                break; // gb
            case 4:
                if (--g == 0) state = 5;
                break;  // b
            case 5:
                if (++r == 255) state = 6;
                break; // rb
            case 6:
                if (++g == 255) state = 7;
                break; // rgb
            case 7:
                if (--g == 0 || --b == 0) state = 1;
                break; // black
        }
        blfb->flush();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));
    }
}

void third(uint32_t *buffer, int pitch) {


//    auto ** ten_pics = new unsigned char*[10];
//    for(int i = 0; i < 10; i++){
//        ten_pics[i] = stbi_load("/user/pic/test.jpg", &width, &height, &channels, 0);
//        print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
//    }

//    char** bigBuffers = new char*[10];
//    for (int i = 0; i < 10; i++) {
//        bigBuffers[i] = new char[1920 * 1080];
//        if (bigBuffers[i] == nullptr) {
//            print("Failed to allocate buffer " << i);
//            // Clean up previously allocated buffers
//            for (int j = 0; j < i; j++) {
//                delete[] bigBuffers[j];
//            }
//            delete[] bigBuffers;
//            return;
//        }
//    }
//
//    print("Successfully allocated 10 buffers of 1920x1080 pixels");

//    // Clean up
//    for (int i = 0; i < 10; i++) {
//        delete[] bigBuffers[i];
//    }
//    delete[] bigBuffers;



    unsigned char *data = stbi_load("/user/pic/test.jpg", &width, &height, &channels, 0);
//    unsigned char *data = stbi_load("/user/pic/ex.png", &width, &height, &channels, 0);
    if (data == nullptr) {
        print("Failed to load image: " << stbi_failure_reason());
        return;
    }

    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
    int x = blfb->getResolutionX();
    int y = blfb->getResolutionY();
    int *picData = new int32_t[x * y];


    while (running) {
        if (changed) {
            calcPicData(picData, data);
            changed = false;
        }
        // Draw image
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                int offset = i + j * (pitch / 4);
                uint32_t rgbColor = picData[i + j * x];

                // Invert color if within mouse cursor area
                if (abs(i - mouseX) < 10 && abs(j - mouseY) < 10) {
                    rgbColor = ~rgbColor | 0xFF000000;  // Invert color, keep alpha
                    if (leftButtonPressed) {
                        rgbColor = 0xFFFF0000; // Red when clicked
                    }
                }

                reinterpret_cast<uint32_t *>(buffer)[offset] = rgbColor;
            }
        }
        blfb->flush();
        checkMouseInput();
        checkKeyboardInput();

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(16));  // ~60 FPS
    }

    if(save){
        stbi_write_png("/user/pic/test_saved.png", x, y, 4, picData, x * 4);
    }

    stbi_image_free(data);
    delete[] picData;
}

void calcPicData(int *picData, const unsigned char *data) {
    const double M_PI = 3.14159265358979323846;
    int centerX = posX + width * scale / 2;
    int centerY = posY + height * scale / 2;
    double rotationRad = rotationDegree * M_PI / 180.0;
    double cosTheta = cos(rotationRad);
    double sinTheta = sin(rotationRad);

    int resX = blfb->getResolutionX();
    int resY = blfb->getResolutionY();
    double invScale = 1.0 / scale;
    int halfWidth = width / 2;
    int halfHeight = height / 2;

    for (int y = 0; y < resY; y++) {
        for (int x = 0; x < resX; x++) {
            int dx = x - centerX;
            int dy = y - centerY;

            int srcX = static_cast<int>((dx * cosTheta + dy * sinTheta) * invScale + halfWidth);
            int srcY = static_cast<int>((dy * cosTheta - dx * sinTheta) * invScale + halfHeight);

            int destIndex = y * resX + x;

            if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                int srcIndex = ((height - 1 - srcY) * width + srcX) * channels;

                picData[destIndex] = (0xff << 24) |
                                     (data[srcIndex] << 16) |
                                     (data[srcIndex + 1] << 8) |
                                     data[srcIndex + 2];
            } else {
                picData[destIndex] = 0xFF000000; // Black for areas outside the image
            }
        }
    }
}

void fourth(uint32_t *buffer, int pitch) {
    unsigned char *data = stbi_load("/user/pic/test_saved.png", &width, &height, &channels, 0);
    if (data == nullptr) {
        print("Failed to load image: " << stbi_failure_reason());
        return;
    }

    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);

    int x = blfb->getResolutionX();
    int y = blfb->getResolutionY();

    // Draw image
    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            int srcX = i * width / x;
            int srcY = j * height / y;
            int srcIndex = (srcY * width + srcX) * channels;
            int destIndex = i + j * (pitch / 4);

            uint32_t rgbColor = (0xff << 24) |
                                (data[srcIndex] << 16) |
                                (data[srcIndex + 1] << 8) |
                                data[srcIndex + 2];

            reinterpret_cast<uint32_t *>(buffer)[destIndex] = rgbColor;
        }
    }

    blfb->flush();

    // Wait for key press
    while (Util::System::in.read() <= 0) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(100));
    }

    stbi_image_free(data);
}

void fifth(uint32_t *buffer, int pitch, StringDrawer &stringDrawer) {
    int x = blfb->getResolutionX();
    int y = blfb->getResolutionY();

    for (int j = 0; j < y; j++) {
        for (int i = 0; i < x; i++) {
            uint32_t color;
            if ((i <= 200 && j <= 600)) {
                color = 0xFFFFFFFF;  // White
            } else {
                color = 0xFF000000;  // Black
            }
            int offset = i + j * (pitch / 4);
            reinterpret_cast<uint32_t *>(buffer)[offset] = color;
        }
    }

    stringDrawer.drawString(Fonts::TERMINAL_8x16, 10, 7, "hhuOS - Image Edit", Color(0, 0, 0), Color(255, 255, 255));
    stringDrawer.drawString(Fonts::TERMINAL_8x16, 200, 7, "Export", Color(0, 0, 0), Color(255, 255, 255));
    stringDrawer.drawString(Fonts::TERMINAL_8x16, 10, 37, "Export Image", Color(0, 0, 0), Color(255, 255, 255));
    stringDrawer.drawString(Fonts::TERMINAL_8x16, 10, 67, "Import Image", Color(0, 0, 0), Color(255, 255, 255));
    stringDrawer.drawString(Fonts::TERMINAL_8x16, 10, 97, "Drawing Tools", Color(0, 0, 0), Color(255, 255, 255));

    blfb->flush();

    // Wait for key press
    while (Util::System::in.read() <= 0) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(100));
    }
}

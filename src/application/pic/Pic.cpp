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
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/font/Terminal8x16.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "Pic.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

using namespace Util;
using namespace Graphic;;

int32_t main(int32_t argc, char *argv[]) {

    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Test\n");
    argumentParser.addArgument("test", false, "t");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl
                            << Util::Io::PrintStream::flush;
        return -1;
    }

    if (argumentParser.hasArgument("number")) {
        auto arg = argumentParser.getArgument("number")[0];
        print("you called with number: " << arg);
    }

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = new LinearFrameBuffer(lfbFile);
//    auto blfb = new BufferedLinearFrameBuffer(lfb, true);
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    auto mouseFile = Util::Io::File("/device/mouse");
    auto mouseInputStream = new Util::Io::FileInputStream(mouseFile);
    mouseInputStream->setAccessMode(Util::Io::File::NON_BLOCKING);

    auto pic = new Pic(lfb, mouseInputStream);
    pic->run();
    delete pic;

    return 0;
}


Pic::Pic(Util::Graphic::LinearFrameBuffer *lfb, Util::Io::FileInputStream *mouseInputStream) {
    this->running = true;
    this->lfb = lfb;
    this->screenX = lfb->getResolutionX();
    this->screenY = lfb->getResolutionY();
    this->pitch = lfb->getPitch();
    this->mouseInputStream = mouseInputStream;
    this->mouse = new MouseInfo();
    this->keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());

    int width, height, channels;
    unsigned char *data = stbi_load("/user/pic/test.jpg", &width, &height, &channels, 0);
    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
    auto *argbData = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        argbData[i] = (0xFF000000 |                   // Alpha: 255
                       (data[i * channels] << 16) |    // Red
                       (data[i * channels + 1] << 8) | // Green
                       data[i * channels + 2]);        // Blue
    }
    auto **layers = new Layer *[6];
    auto *layer1 = new Layer(width, height, 0, 0, argbData);
    auto *layer2 = new Layer(width, height, 50, 0, argbData);
    auto *layer3 = new Layer(width, height, 100, 0, argbData);
    auto *layer4 = new Layer(width, height, 150, 0, argbData);
    auto *layer5 = new Layer(width, height, 200, 0, argbData);
    auto *layer6 = new Layer(width, height, 250, 0, argbData);
    layers[0] = layer1;
    layers[1] = layer2;
    layers[2] = layer3;
    layers[3] = layer4;
    layers[4] = layer5;
    layers[5] = layer6;
    stbi_image_free(data);

    rData = new RenderData(mouse, layers, 6, 3, new RenderFlags());

    auto renderer = new Renderer(rData, lfb);
    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);
}

void Pic::run() {
    while (running) {
        this->checkMouseInput();
        this->checkKeyboardInput();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }
}

void Pic::checkMouseInput() {
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = mouseInputStream->read();
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            auto min = [](int a, int b) { return a < b ? a : b; };
            auto max = [](int a, int b) { return a > b ? a : b; };
            mouse->X = max(0, min(screenX - 1, mouse->X + mouseUpdate.xMovement));
            mouse->Y = max(0, min(screenY - 1, mouse->Y - mouseUpdate.yMovement));
            mouse->leftButtonPressed = mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON;
            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
        }
        value = mouseInputStream->read();
        rData->flags->mouseChanged();
    }
}

void Pic::checkKeyboardInput() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder->parseScancode(scancode)) {
            auto key = keyDecoder->getCurrentKey();
            switch (key.getScancode()) {
                case Util::Io::Key::ESC:
                    running = false;
                    break;
            }
        }
        scancode = Util::System::in.read();
    }
}
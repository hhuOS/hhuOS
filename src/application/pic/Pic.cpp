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
#include "Layer.h"
#include "Renderer.h"

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

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);


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
    auto** layers = new Layer*[2];
    auto *layer1 = new Layer(width, height, 0, 0, argbData);
    auto *layer2 = new Layer(width, height, 200, 0, argbData);
    layers[0] = layer1;
    layers[1] = layer2;
    stbi_image_free(data);

    auto renderer = new Renderer(layers, 2);
    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);

    while (true) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1000));
    }




    return 0;
}

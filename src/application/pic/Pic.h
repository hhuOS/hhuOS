#ifndef PIC_H
#define PIC_H

#include "Layer.h"
#include "Renderer.h"
#include "DataWrapper.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/io/key/KeyDecoder.h"
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
#include "lib/util/collection/Map.h"
#include "Button.h"


class Pic {
public:
    explicit Pic(Util::Graphic::LinearFrameBuffer *lfb,
                 Util::Io::FileInputStream *mouseInputStream);

    ~Pic() = default;

    void run();

//    void test(UiData *data);

private:
    bool running;
    int screenX, screenY, pitch;
    Util::Graphic::LinearFrameBuffer *lfb;
    Util::Io::FileInputStream *mouseInputStream;
    MouseData *mouse;
    Util::Io::KeyDecoder *keyDecoder;
    RenderData *rData;
    UiData *uiData;

    void init_gui();

    void checkMouseInput();

    void checkKeyboardInput();

    void parseMouseToGui();
};

#endif // PIC_H

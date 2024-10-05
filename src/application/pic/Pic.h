#ifndef PIC_H
#define PIC_H

#include "Layer.h"
#include "Renderer.h"
#include "DataWrapper.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/io/key/KeyDecoder.h"


class Pic {
public:
    explicit Pic(Util::Graphic::LinearFrameBuffer *lfb,
                 Util::Io::FileInputStream *mouseInputStream);

    ~Pic() = default;

    void run();

    void checkMouseInput();

    void checkKeyboardInput();

private:
    bool running;
    int screenX, screenY, pitch;
    Util::Graphic::LinearFrameBuffer *lfb;
    Util::Io::FileInputStream *mouseInputStream;
    MouseInfo *mouse;
    Util::Io::KeyDecoder *keyDecoder;
    RenderData *rData;
};

#endif // PIC_H

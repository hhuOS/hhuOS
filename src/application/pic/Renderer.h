//
// Created by Rafael Reip on 04.10.24.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "lib/util/collection/ArrayList.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "Layer.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/io/file/File.h"
#include "lib/util/async/Thread.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/Address.h"
#include "lib/util/time/Timestamp.h"

class Renderer : public Util::Async::Runnable {
public:
    explicit Renderer(Layer ** layers, int layerCount);
    ~Renderer() override = default;
    void run() override;

private:
    int screenX, screenY, pitch;
    Util::Graphic::LinearFrameBuffer * lfb;
    Util::Graphic::BufferedLinearFrameBuffer *blfb;
    Layer ** layers;
    int layerCount;
    uint32_t * buffer;

};

#endif // RENDERER_H

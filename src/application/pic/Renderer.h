//
// Created by Rafael Reip on 04.10.24.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "lib/util/collection/ArrayList.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/io/file/File.h"
#include "lib/util/async/Thread.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/Address.h"
#include "lib/util/time/Timestamp.h"
#include "lib/libc/stdlib.h"
#include "DataWrapper.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/libc/math.h"

#include "GuiLayer.h"
#include "helper.h"
#include "Layers.h"


using namespace Util::Graphic;

//class Renderer : public Util::Async::Runnable {
class Renderer {
public:
    explicit Renderer(DataWrapper *data);

//    ~Renderer() override = default;
    ~Renderer() = default;

//    void run() override;
    void render();

private:
    void prepareBase();

    static uint32_t *newBuffer(int size);

    void renderMouse();

    void removeMouse();

    void renderResult();

    void renderWorkArea();

    void renderGui();

    void renderOverlay();

    void drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color color);

    void drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color c1, Color c2, Color c3, Color c4);

    void renderLayers();

    DataWrapper *data;
    uint32_t *buff_lfb;
    uint32_t *buff_result;
    uint32_t *buff_base, *buff_workarea, *buff_gui;
    uint32_t *buff_overlay, *buff_layers;
    uint32_t *buff_under_current, *buff_over_current;
    Color cblack, cgreen, cwhite, cred;
    int lastRenderedMouseX, lastRenderedMouseY;
    // for overlay
    LinearFrameBuffer *lfb;
    PixelDrawer *pixelDrawer;
    LineDrawer *lineDrawer;
    StringDrawer *stringDrawer;
};

#endif // RENDERER_H

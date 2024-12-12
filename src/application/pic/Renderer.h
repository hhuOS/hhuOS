//
// Created by Rafael Reip on 04.10.24.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <lib/libc/time.h>
#include "lib/util/graphic/Color.h"
#include "lib/util/base/String.h"

namespace Util::Graphic {
    class LinearFrameBuffer;

    class PixelDrawer;

    class LineDrawer;

    class StringDrawer;
}

class DataWrapper;

class Renderer {
public:
    explicit Renderer(DataWrapper *data);

    ~Renderer();

    void render();

    void setDebugString(Util::String debugString);

private:
    void prepareBase();

    static uint32_t *newBuffer(int size);

    void renderMouse();

    void removeMouse();

    void renderResult();

    void renderWorkArea();

    void renderGui();

    void renderOverlay();

    void renderToolsOverlay();

    void drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Util::Graphic::Color color);

    void drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,
                        Util::Graphic::Color c1, Util::Graphic::Color c2, Util::Graphic::Color c3, Util::Graphic::Color c4);

    void drawFilledOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint32_t color);

    void renderLayers();

    DataWrapper *data;
    uint32_t *buff_lfb;
    uint32_t *buff_result;
    uint32_t *buff_base, *buff_workarea, *buff_gui;
    uint32_t *buff_overlay, *buff_layers;
    uint32_t *buff_under_current, *buff_over_current;
    Util::Graphic::Color cblack, cgreen, cwhite, cred, cgray;
    int lastRenderedMouseX, lastRenderedMouseY;
    bool usingBufferedBuffer;
    // for overlay
    Util::Graphic::LinearFrameBuffer *lfb_overlay;
    Util::Graphic::PixelDrawer *pixelDrawer_overlay;
    Util::Graphic::LineDrawer *lineDrawer_overlay;
    Util::Graphic::StringDrawer *stringDrawer_overlay;
    Util::String debugString;
    // fps
    Util::Graphic::PixelDrawer *pixelDrawer_lfb;
    Util::Graphic::StringDrawer *stringDrawer_lfb;
    Util::Graphic::PixelDrawer *pixelDrawer_blfb;
    Util::Graphic::StringDrawer *stringDrawer_blfb;
    time_t lastTime;
    int frames, fps;
};

#endif // RENDERER_H

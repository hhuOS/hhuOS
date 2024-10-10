//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H

#include "Layer.h"
#include "GuiLayer.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Map.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/key/MouseDecoder.h"


class RenderFlags {
public:

    bool anyChange = true;
    bool result = true;
    bool mouse = true;
    bool gui = true;
    bool workArea = true;
    bool base = true;
    bool overlay = true;
    bool layers = true;
    bool layerOrder = true;

    void mouseChanged() { anyChange = true, mouse = true; }

    void guiChanged() { anyChange = true, result = true, gui = true; }

    void baseChanged() { anyChange = true, result = true, base = true; }

    void overlayChanged() { anyChange = true, result = true, workArea = true, overlay = true; }

    void currentLayerChanged() { anyChange = true, result = true, workArea = true, layers = true; }

    void layerOrderChanged() { anyChange = true, result = true, workArea = true, layers = true, layerOrder = true; }

};

enum Tool {
    NONE, MOVE, ROTATE, SCALE, CROP, PEN, ERASER, COLOR_PICKER,
};

class DataWrapper {
public:
    DataWrapper();

    ~DataWrapper() = default;

    // screen
    Util::Graphic::LinearFrameBuffer *lfb;
    int screenX, screenY, pitch, screenAll;
    int workAreaX, workAreaY, workAreaAll;
    int guiX, guiY, guiAll;

    // input
    Util::Io::FileInputStream *mouseInputStream;
    Util::Io::KeyDecoder *keyDecoder;
    int mouseX, mouseY, oldMouseX, oldMouseY;
    bool leftButtonPressed, oldLeftButtonPressed;

    // rendering
    RenderFlags *flags = new RenderFlags();

    // layers
    Layer **layers;
    int layerCount;
    int currentLayer;

    // gui
    Util::Map<Util::String, GuiLayer *> *guiLayers;
    GuiLayer *currentGuiLayer;

    // work vars
    bool running;
    Tool currentTool;
    int moveX, moveY;
    int rotateDeg;
    double scale;
    int cropLeft, cropRight, cropTop, cropBottom;
    int penSize;
    uint32_t penColor;
};

#endif // HHUOS_DATAWRAPPER_H

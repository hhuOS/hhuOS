//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H

#include "Layer.h"
#include "GuiLayer.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Map.h"
#include "lib/util/graphic/LinearFrameBuffer.h"


class MouseData {
public:
    MouseData() : X(0), Y(0), leftButtonPressed(false) {}

    int X, Y;
    bool leftButtonPressed;
};

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

    void mouseChanged() {
        anyChange = true, mouse = true;
    }

    void guiChanged() {
        anyChange = true, result = true, gui = true;
    }

    void baseChanged() {
        anyChange = true, result = true, base = true;
    }

    void overlayChanged() {
        anyChange = true, result = true, workArea = true, overlay = true;
    }

    void currentLayerChanged() {
        anyChange = true, result = true, workArea = true, layers = true;
    }

    void layerOrderChanged() {
        anyChange = true, result = true, workArea = true, layers = true, layerOrder = true;
    }

};

class RenderData {
public:
    explicit RenderData(MouseData *mouse, Layer **layers, int layerCount, int currentLayer, RenderFlags *flags,
                        UiData *uiData)
            : mouse(mouse), layers(layers), layerCount(layerCount), currentLayer(currentLayer), flags(flags),
              uiData(uiData) {}

    MouseData *mouse;
    Layer **layers;
    int layerCount;
    int currentLayer;
    RenderFlags *flags;
    UiData *uiData;
};

enum Tool {
    NONE,
    MOVE,
    ROTATE,
    SCALE,
    CROP,
    PEN,
    ERASER,
    COLOR_PICKER,
};

class UiData {
public:
    explicit UiData(Util::Map<Util::String, GuiLayer *> *guiLayers) : guiLayers(guiLayers), currentGuiLayer(nullptr),
                                                                      currentTool(NONE), moveX(0), moveY(0),
                                                                      rotateDeg(0), scale(1.0), cropLeft(0),
                                                                      cropRight(0), cropTop(0), cropBottom(0),
                                                                      penSize(1),
                                                                      penColor(0x00000000) {}

    Util::Map<Util::String, GuiLayer *> *guiLayers;
    GuiLayer *currentGuiLayer;
    Tool currentTool;
    int moveX, moveY;
    int rotateDeg;
    double scale;
    int cropLeft, cropRight, cropTop, cropBottom;
    int penSize;
    uint32_t penColor;
};

class DataWrapper {
public:
    DataWrapper();

    // screen
    Util::Graphic::LinearFrameBuffer *lfb;
    int screenX, screenY, pitch;
    int workAreaX, workAreaY;
    int guiX, guiY;

    // input
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
    Tool currentTool;
    int moveX, moveY;
    int rotateDeg;
    double scale;
    int cropLeft, cropRight, cropTop, cropBottom;
    int penSize;
    uint32_t penColor;
};

#endif // HHUOS_DATAWRAPPER_H

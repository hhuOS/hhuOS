//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H

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
#include "lib/util/collection/Queue.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "MessageHandler.h"

#define  PI 3.14159265358979323846

// Forward declaration since otherwise circular dependency
class Layers;

class History;

class GuiLayer;

class Button;

class RenderFlags;

class MessageHandler;

enum Tool {
    // tools
    NOTHING = 0, MOVE = 1, ROTATE = 2, SCALE = 3, CROP = 4, PEN = 5, ERASER = 6, COLOR = 7, SHAPE = 8, REPLACE_COLOR = 15,
    // layer tools
    EXPORT_PNG = 9, EXPORT_JPG = 10, EXPORT_BMP = 11, NEW_EMPTY = 12, COMBINE = 13, DUPLICATE = 14,
};

enum ToolCorner {
    TOP_LEFT = 0, TOP_RIGHT = 1, BOTTOM_LEFT = 2, BOTTOM_RIGHT = 3,
};

enum Shape {
    DEFAULT = 0, RECTANGLE = 1, SQUARE = 2, CIRCLE = 3, ELLIPSE = 4,
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
    int xMovement, yMovement;
    int mouseX, mouseY, oldMouseX, oldMouseY;
    bool leftButtonPressed, oldLeftButtonPressed, newlyPressed;
    Util::Queue<Util::Pair<int, int>> *mouseClicks;
    bool clickStartedOnGui;
    int lastInteractedButton;
    Util::String *currentInput;
    bool captureInput;
    int16_t lastScancode;

    // rendering
    RenderFlags *flags;
    MessageHandler *mHandler;

    // layers
    Layers *layers;
    History *history;

    // gui
    Util::Map<Util::String, GuiLayer *> *guiLayers;
    GuiLayer *currentGuiLayer;
    GuiLayer *currentGuiLayerBottom;
    Button *textButton;

    // overlay
    const char *debugString;

    // work vars
    bool running;
    Tool currentTool;
    int moveX, moveY;
    int rotateDeg;
    double scale;
    ToolCorner toolCorner;
    int cropLeft, cropRight, cropTop, cropBottom;
    int penSize;
    int colorA, colorR, colorG, colorB;
    int combineFirst, combineSecond;
    int layerX, layerY, layerW, layerH; // for layer export / new empty layer (LAYER-Tool)
    int dupeIndex;
    int shapeX, shapeY, shapeW, shapeH;
    Shape currentShape;
    int replaceColorX, replaceColorY;
    double replaceColorTolerance;
};


class RenderFlags {
public:

    bool anyChange = true;
    bool result = true;
    bool mouse = true;
    bool gui = true;
    bool guiLayer = true;
    bool guiJustButton = true;
    bool workArea = true;
    bool base = true;
    bool overlay = true;
    bool messages = true;
    bool layers = true;
    bool layerOrder = true;

    void mouseChanged() { anyChange = true, mouse = true; }

    void guiButtonChanged() { anyChange = true, result = true, gui = true, guiJustButton = true; }

    void guiLayerChanged() { anyChange = true, result = true, gui = true, guiLayer = true; }

    void baseChanged() { anyChange = true, result = true, base = true; }

    void overlayChanged() { anyChange = true, result = true, workArea = true, overlay = true; }

    void messagesChanged() { anyChange = true, result = true, workArea = true, messages = true; }

    void currentLayerChanged() { anyChange = true, result = true, workArea = true, layers = true; }

    void layerOrderChanged() { anyChange = true, result = true, workArea = true, overlay = true, layers = true, layerOrder = true; }

};

#endif // HHUOS_DATAWRAPPER_H

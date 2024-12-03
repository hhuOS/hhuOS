//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H

#include "lib/util/base/String.h"
#include "lib/util/collection/Map.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
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
#include "Settings.h"

#define  PI 3.14159265358979323846

// Forward declaration since otherwise circular dependency
class Layers;

class History;

class GuiLayer;

class Button;

class RenderFlags;

class MessageHandler;

class Settings;

enum Tool {
    // tools
    NOTHING, MOVE, ROTATE, SCALE, CROP, PEN, ERASER, COLOR, SHAPE, REPLACE_COLOR,
    // layer tools
    EXPORT_PNG, EXPORT_JPG, EXPORT_BMP, NEW_EMPTY, COMBINE, DUPLICATE,
};

enum ToolCorner {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT,
};

enum Shape {
    DEFAULT, RECTANGLE, SQUARE, CIRCLE, ELLIPSE,
};

class DataWrapper {
public:
    DataWrapper(Util::Io::File *lfbFile);

    ~DataWrapper() = default;

    // screen
    Util::Graphic::LinearFrameBuffer *lfb;
    Util::Graphic::BufferedLinearFrameBuffer *blfb;
    int screenX, screenY, pitch, screenAll;
    int workAreaX, workAreaY, workAreaAll;
    int guiX, guiY, guiAll;
    int buttonCount;

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
    bool inMainMenu;

    // overlay
    const char *debugString;

    // settings
    Settings *settings;

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

    void
    allChanged() { anyChange = true, result = true, mouse = true, gui = true, guiLayer = true, guiJustButton = true, workArea = true, base = true, overlay = true, messages = true, layers = true, layerOrder = true; }

};

#endif // HHUOS_DATAWRAPPER_H

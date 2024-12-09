//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H

#include "helper.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/collection/HashMap.h"

namespace Util {
    class String;

    namespace Io {
        class File;

        class FileInputStream;

        class KeyDecoder;
    }
    namespace Graphic {
        class LinearFrameBuffer;

        class BufferedLinearFrameBuffer;

        class PixelDrawer;

        class LineDrawer;

        class StringDrawer;
    }
}

class Layers;

class GuiLayer;

class Button;

class RenderFlags;

class Settings;

class MessageHandler;

class History;

class DataWrapper {
public:
    explicit DataWrapper(Util::Io::File *lfbFile);

    ~DataWrapper();

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
    int mouseX, mouseY;
    bool leftButtonPressed, oldLeftButtonPressed, newlyPressed;
    Util::ArrayBlockingQueue<Util::Pair<int, int>> *mouseClicks;
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
    Util::HashMap<Util::String, GuiLayer *> *guiLayers;
    GuiLayer *currentGuiLayer;
    GuiLayer *currentGuiLayerBottom;
    Button *textButton;
    bool inMainMenu;

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

#endif // HHUOS_DATAWRAPPER_H

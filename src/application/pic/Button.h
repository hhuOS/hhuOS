//
// Created by Rafael Reip on 06.10.24.
//

#ifndef HHUOS_BUTTON_H
#define HHUOS_BUTTON_H

#include <cstdint>
#include "Layer.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "lib/util/graphic/font/Sun12x22.h"
#include "lib/util/graphic/LineDrawer.h"


#include "DataWrapper.h"

using namespace Util::Graphic;

enum ButtonType {
    NONE,
    METHOD,
    INT_VALUE,
    DOUBLE_VALUE,
    CONFIRM,
    COLOR,
    LAYER,
    INPUT
};

class Button {
public:

    explicit Button(DataWrapper *data);

    ~Button() = default;

    void processClick(int relX, int relY);

    void showClick(int relX, int relY);

    void showHover(int relX, int relY);

    void removeInteraction();

    uint32_t *getBuffer();

    Button *setInfo(const char *string);

    Button *setHotkey(char hotkey);

    Button *setMethodButton(void (*method)(DataWrapper *));

    Button *setIntValueButton(int *value);

    Button *setDoubleValueButton(double *dvalue);

    Button *setConfirmButton(void (*cancel)(DataWrapper *), void (*ok)(DataWrapper *));

    Button *setColorButton(uint32_t *color);

    Button *setLayerButton(Layer **layer, int layerCount, int whichLayer);

    Button *setInputButton(Util::String *input, bool *captureInput);

    Button *setRenderFlagMethod(void (RenderFlags::*rFlagMethod)());

    Button *changeGreenIfTool(Tool tool);

    bool bufferChanged;

    void render();

private:

    void renderBorder(uint32_t color);

    void renderBackground(int x1, int x2, uint32_t color);

    void renderMethod();

    void renderValue(const char *text);

    void renderIntValue();

    void renderDoubleValue();

    void renderConfirm();

    void renderColor();

    void renderLayer();

    void renderInput();

    DataWrapper *data;
    uint32_t *buffer;
    LinearFrameBuffer *lfb;
    PixelDrawer *pixelDrawer;
    LineDrawer *lineDrawer;
    StringDrawer *stringDrawer;
    Color cblack, cgray, cgreen, cdarkgray, cred;
    uint32_t black, gray, green, darkgray, red;
    ButtonType type;
    bool click;
    bool hover;
    int mouseX, mouseY;

    // optionals
    const char *info;
    char hotkey;
    bool hasHotkey;
    int *intValue = nullptr;
    double *doubleValue = nullptr;
    Util::String *input = nullptr;
    bool *captureInput = nullptr;
    Tool setGreenTool = Tool::NOTHING;

    void (*method1)(DataWrapper *data) = nullptr;

    void (*method2)(DataWrapper *data) = nullptr;

    void (*method3)(DataWrapper *data) = nullptr;

    void (RenderFlags::*rFlagMethod)() = nullptr;

};

#endif // HHUOS_BUTTON_H

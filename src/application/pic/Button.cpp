//
// Created by Rafael Reip on 06.10.24.
//

#include "Button.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "lib/util/graphic/font/Sun12x22.h"
#include "lib/util/graphic/LineDrawer.h"
#include "string.h"

#include "GuiLayer.h"
#include "Layers.h"
#include "Layer.h"
#include "DataWrapper.h"
#include "Settings.h"

/**
 * \brief Constructor for the Button class.
 *
 * Initializes the Button with the provided DataWrapper, sets up the buffer,
 * and initializes various graphic components for rendering.
 *
 * \param data Pointer to the DataWrapper object.
 */
Button::Button(DataWrapper *data) {
    this->data = data;
    this->buffer = new uint32_t[30 * 200];
    for (int i = 0; i < 30 * 200; i++) {
        this->buffer[i] = 0x00000000;
    }
    this->lfb = new Util::Graphic::LinearFrameBuffer(this->buffer, 200, 30, 32, 200 * 4, false);
    this->pixelDrawer = new Util::Graphic::PixelDrawer(*lfb);
    this->lineDrawer = new Util::Graphic::LineDrawer(*pixelDrawer);
    this->stringDrawer = new Util::Graphic::StringDrawer(*pixelDrawer);
    this->bufferChanged = true;
    this->click = false;
    this->hover = false;
    this->type = NONE;
    this->info = "Button";
    this->hotkey = 0;
    this->hasHotkey = false;
    this->cblack = Util::Graphic::Color(0, 0, 0);
    this->cgray = Util::Graphic::Color(128, 128, 128);
    this->cdarkgray = Util::Graphic::Color(80, 80, 80);
    this->cgreen = Util::Graphic::Color(0, 255, 0);
    this->cred = Util::Graphic::Color(255, 0, 0);
    this->black = cblack.getRGB32();
    this->gray = cgray.getRGB32();
    this->darkgray = cdarkgray.getRGB32();
    this->green = cgreen.getRGB32();
    this->red = cred.getRGB32();
    this->mouseX = 0;
    this->mouseY = 0;
    this->bitmap = nullptr;
    render();
}

/**
 * \brief Destructor for the Button class.
 */
Button::~Button() {
//    delete[] buffer; // dont delete, since lfb destructor will delete it
    delete lfb;
    delete pixelDrawer;
    delete lineDrawer;
    delete stringDrawer;
}

/**
 * \brief Sets optional hotkey rendering for the Button.
 *
 * \param h The character representing the hotkey.
 * \return A pointer to the Button object.
 */
Button *Button::setHotkey(char h) {
    this->hotkey = h;
    return this;
}

/**
 * \brief Sets optional bitmap rendering for the Button.
 *
 * \param b Pointer to the bitmap data.
 * \return A pointer to the Button object.
 */
Button *Button::set16Bitmap(uint8_t *b) {
    this->bitmap = b;
    return this;
}

/**
 * \brief Sets optional information string to be displayed on the Button.
 *
 * \param string Pointer to the information string.
 * \return A pointer to the Button object.
 */
Button *Button::setInfo(const char *string) {
    this->info = string;
    return this;
}

/**
 * \brief Sets optional render flag method for the Button that is called on click.
 *
 * \param m Pointer to the RenderFlags method.
 * \return A pointer to the Button object.
 */
Button *Button::setRenderFlagMethod(void (RenderFlags::*m)()) {
    this->rFlagMethod = m;
    return this;
}

/**
 * \brief Sets optional second render flag method for the Button that is called on click.
 *
 * \param m Pointer to the RenderFlags method.
 * \return A pointer to the Button object.
 */
Button *Button::setSecondRenderFlagMethod(void (RenderFlags::*m)()) {
    this->rFlagMethod2 = m;
    return this;
}

/**
 * \brief Sets the Button to be a method button.
 *
 * The specified method is called on click.
 *
 * \param method Pointer to the method to be called on click.
 * \return A pointer to the Button object.
 */
Button *Button::setMethodButton(void (*method)(DataWrapper *)) {
    this->type = METHOD;
    this->method1 = method;
    render();
    return this;
}

/**
 * \brief Sets the Button to be an integer value button.
 *
 * The integer value is modified on click by 1.
 *
 * \param value Pointer to the integer value.
 * \return A pointer to the Button object.
 */
Button *Button::setIntValueButton(int *value) {
    this->type = INT_VALUE;
    this->intValue = value;
    render();
    return this;
}

/**
 * \brief Sets the Button to be an integer value button with limits.
 *
 * The integer value is modified on click by 1.
 *
 * \param value Pointer to the integer value.
 * \param limit_low The lower limit for the integer value.
 * \param limit_high The upper limit for the integer value.
 * \return A pointer to the Button object.
 */
Button *Button::setIntValueButton(int *value, int limit_low, int limit_high) {
    this->type = INT_VALUE;
    this->intValue = value;
    this->intLimitLow = limit_low;
    this->intLimitHigh = limit_high;
    this->hasIntLimits = true;
    render();
    return this;
}

/**
 * \brief Sets the Button to be a double value button.
 *
 * The double value is modified on click by 0.05.
 *
 * \param dvalue Pointer to the double value.
 * \return A pointer to the Button object.
 */
Button *Button::setDoubleValueButton(double *dvalue) {
    this->type = DOUBLE_VALUE;
    this->doubleValue = dvalue;
    render();
    return this;
}

/**
 * \brief Sets the Button to be a double value button with limits.
 *
 * The double value is modified on click by 0.05.
 *
 * \param dvalue Pointer to the double value.
 * \param limit_low The lower limit for the double value.
 * \param limit_high The upper limit for the double value.
 * \return A pointer to the Button object.
 */
Button *Button::setDoubleValueButton(double *dvalue, double limit_low, double limit_high) {
    this->type = DOUBLE_VALUE;
    this->doubleValue = dvalue;
    this->doubleLimitLow = limit_low;
    this->doubleLimitHigh = limit_high;
    this->hasDoubleLimits = true;
    render();
    return this;
}

/**
 * \brief Sets the Button to be a boolean value button.
 *
 * The boolean value is toggled on click.
 *
 * \param bvalue Pointer to the boolean value.
 * \return A pointer to the Button object.
 */
Button *Button::setBooleanButton(bool *bvalue) {
    this->type = BOOLEAN;
    this->boolValue = bvalue;
    render();
    return this;
}

/**
 * \brief Sets the Button to be an input button.
 *
 * The input string is cleared by clicking the left side and the captureInput flag is toggled by clicking the right side.
 *
 * \param s Pointer to the input string.
 * \param capture Pointer to the captureInput flag.
 * \return A pointer to the Button object.
 */
Button *Button::setInputButton(Util::String *s, bool *capture) {
    this->type = INPUT;
    this->input = s;
    this->captureInput = capture;
    render();
    return this;
}

/**
 * \brief Sets the Button to be a confirm button.
 *
 * The first method is called on click on the left side and the second method is called on click on the right side.
 *
 * \param cancel Pointer to the cancel method.
 * \param ok Pointer to the ok method.
 * \return A pointer to the Button object.
 */
Button *Button::setConfirmButton(void (*cancel)(DataWrapper *), void (*ok)(DataWrapper *)) {
    this->type = CONFIRM;
    this->method1 = cancel;
    this->method2 = ok;
    render();
    return this;
}

/**
 * \brief Sets the Button to be a layer button.
 *
 * The layerNum is used to identify the layer to be modified.
 *
 * \param num The number of the layer.
 * \return A pointer to the Button object.
 */
Button *Button::setLayerButton(int num) {
    this->type = LAYER;
    this->layerNum = num;
    render();
    return this;
}

/**
 * \brief Renders the Button Border green if the current tool is the specified tool.
 */
Button *Button::changeGreenIfTool(Tool tool) {
    this->setGreenTool = tool;
    render();
    return this;
}

/**
 * \brief Renders the Button Border green if the current shape is the specified shape.
 */
Button *Button::changeGreenIfShape(Shape shape) {
    setGreenShape = shape;
    render();
    return this;
}

/**
 * \brief Sets the color of the Button to the specified ARGB values.
 */
Button *Button::setColor(int *a, int *r, int *g, int *b) {
    colorA = a, colorR = r, colorG = g, colorB = b;
    showcolor = true;
    render();
    return this;
}

/**
 * \brief Sets the Button to appear the top GUI layer on change.
 */
Button *Button::setAppearTopOnChange(bool set) {
    this->appearTopOnChange = set;
    return this;
}

/**
 * \brief Sets the Button to appear the bottom GUI layer on change.
 */
Button *Button::setAppearBottomOnChange(bool set) {
    this->appearBottomOnChange = set;
    return this;
}

/**
 * \brief Returns the render buffer of the Button.
 *
 * \return Pointer to the buffer.
 */
uint32_t *Button::getBuffer() {
    return this->buffer;
}

/**
 * \brief Processes a click event on the Button.
 *
 * Updates the Button state based on the relative click coordinates and the Button type.
 * Executes associated methods or modifies values as needed.
 * Renders the Button after processing the click.
 *
 * \param relX The relative X coordinate of the click.
 * \param relY The relative Y coordinate of the click.
 */
void Button::processClick(int relX, int relY) {
    this->mouseX = relX;
    this->mouseY = relY;
    switch (type) {
        case METHOD:
            method1(data);
            break;
        case INT_VALUE:
            if (relX < 40) {
                *intValue -= 1;
            } else if (relX > 160) {
                *intValue += 1;
            } else {
                if (data->currentInput->length() > 0) {
                    *intValue = Util::String::parseInt(*data->currentInput);
                    if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                } else {
                    *intValue = 0;
                }
            }
            if (hasIntLimits) {
                if (*intValue > intLimitHigh) *intValue = intLimitHigh;
                if (*intValue < intLimitLow) *intValue = intLimitLow;
            }
            break;
        case DOUBLE_VALUE:
            if (relX < 40) {
                *doubleValue -= 0.05;
            } else if (relX > 160) {
                *doubleValue += 0.05;
            } else {
                if (data->currentInput->length() > 0) {
                    *doubleValue = Util::String::parseDouble(*data->currentInput);
                    if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                } else {
                    *doubleValue = 0.01;
                }
            }
            if (hasDoubleLimits) {
                if (*doubleValue > doubleLimitHigh) *doubleValue = doubleLimitHigh;
                if (*doubleValue < doubleLimitLow) *doubleValue = doubleLimitLow;
            }
            break;
        case BOOLEAN:
            *boolValue = !*boolValue;
            break;
        case CONFIRM:
            if (relX < 100) {
                method1(data);
            } else {
                method2(data);
            }
            break;
        case LAYER:
            if (layerNum > data->layers->countLayersNum()) return;
            if (relX < 40) {
                data->layers->setCurrent(layerNum);
            } else if (relX < 80) {
                data->layers->swap(layerNum, layerNum - 1);
                if (layerNum == data->layers->currentLayerNum()) data->layers->setCurrent(layerNum - 1);
            } else if (relX < 120) {
                data->layers->swap(layerNum, layerNum + 1);
                if (layerNum == data->layers->currentLayerNum()) data->layers->setCurrent(layerNum + 1);
            } else if (relX < 160) {
                data->layers->deletetAt(layerNum);
            } else {
                data->layers->changeVisibleAt(layerNum);
            }
            break;
        case INPUT:
            if (relX < 40) {
                *input = Util::String();
            } else {
                *captureInput = !*captureInput;
            }
            break;
        case NONE: // do nothing
            break;
    }
    if (rFlagMethod != nullptr) {
        (data->flags->*rFlagMethod)();
    }
    if (rFlagMethod2 != nullptr) {
        (data->flags->*rFlagMethod2)();
    }
    if (appearTopOnChange) {
        data->currentGuiLayer->appear();
    }
    if (appearBottomOnChange) {
        data->currentGuiLayerBottom->appear();
    }
    render();
}

/**
 * \brief Shows the Button as clicked.
 *
 * Updates the Button state to reflect a click event and triggers a render.
 *
 * \param relX The relative X coordinate of the click.
 * \param relY The relative Y coordinate of the click.
 */
void Button::showClick(int relX, int relY) {
    this->mouseX = relX;
    this->mouseY = relY;
    click = true;
    hover = false;
    render();
}

/**
 * \brief Shows the Button as hovered.
 *
 * Updates the Button state to reflect a hover event and triggers a render.
 *
 * \param relX The relative X coordinate of the hover.
 * \param relY The relative Y coordinate of the hover.
 */
void Button::showHover(int relX, int relY) {
    this->mouseX = relX;
    this->mouseY = relY;
    hover = true;
    click = false;
    render();
}

/**
 * \brief Removes any interaction state from the Button.
 *
 * Resets the Button state to reflect no interaction and triggers a render.
 */
void Button::removeInteraction() {
    hover = false;
    click = false;
    render();
}

/**
 * \brief Renders the Button based on its type.
 *
 * Calls the appropriate render method for the Button type.
 * Sets the bufferChanged flag to true and notifies the GUI that the Button has changed.
 */
void Button::render() {
    switch (type) {
        case METHOD:
            renderMethod();
            break;
        case INT_VALUE:
            renderIntValue();
            break;
        case DOUBLE_VALUE:
            renderDoubleValue();
            break;
        case BOOLEAN:
            renderBoolean();
            break;
        case CONFIRM:
            renderConfirm();
            break;
        case LAYER:
            renderLayer();
            break;
        case INPUT:
            renderInput();
            break;
        default:
            for (int i = 0; i < 30 * 200; i++) {
                this->buffer[i] = 0xFFFF0000; // red to warn
            }
            break;
    }
    bufferChanged = true;
    data->flags->guiButtonChanged();
}

/**
 * \brief Renders the border of the Button with the specified color.
 *
 * \param color The color to use for the border.
 */
void Button::renderBorder(uint32_t color) {
    int border = 2;
    for (int i = 0; i < border * 200; i++) { // top border
        this->buffer[i] = color;
    }
    for (int i = (30 - border) * 200; i < 30 * 200; i++) { // bottom border
        this->buffer[i] = color;
    }
    for (int i = 0; i < 30 * 200; i += 200) { // left and right border
        for (int j = 0; j < border; j++) {
            this->buffer[i + j] = color;
            this->buffer[i + 200 - j - 1] = color;
        }
    }
}

/**
 * \brief Renders the background of the Button with the specified color.
 *
 * \param x1 The starting X coordinate.
 * \param x2 The ending X coordinate.
 * \param color The color to use for the background.
 */
void Button::renderBackground(int x1, int x2, uint32_t color) {
    for (int i = 0; i < 30 * 200; i += 200) {
        for (int j = i + x1; j < i + x2; j++) {
            this->buffer[j] = color;
        }
    }
}

/**
 * \brief Renders the Button when it is of type METHOD.
 */
void Button::renderMethod() {
    renderBackground(0, 200, click ? green : hover ? darkgray : gray);
    int xStringpos = 100 - (strlen(info) * 4);
    if (showcolor) {
        uint32_t color = 0xFF000000 | (*colorR << 16) | (*colorG << 8) | *colorB;
        renderBackground(0, 80, color);
        Util::Graphic::Color c = this->setGreenTool == data->currentTool ? cgreen : cblack;
        if (c != cgreen && setGreenShape != Shape::DEFAULT) {
            c = setGreenShape == data->currentShape ? cgreen : cblack;
        }
        lineDrawer->drawLine(79, 0, 79, 30, c);
        lineDrawer->drawLine(80, 0, 80, 30, c);
        xStringpos = 140 - (strlen(info) * 4);
        if (bitmap != nullptr) {
            stringDrawer->drawMonoBitmap(90, 7, 16, 16, cblack, click ? cgreen : hover ? cdarkgray : cgray, bitmap);
        }
    } else if (bitmap != nullptr) {
        stringDrawer->drawMonoBitmap(10, 7, 16, 16, cblack, click ? cgreen : hover ? cdarkgray : cgray, bitmap);
    }
    if (hotkey != 0) {
        stringDrawer->drawMonoBitmap(180, 7, 16, 16, cblack, click ? cgreen : hover ? cdarkgray : cgray, Bitmaps::brackets);
        char hotkeyString[2] = {hotkey, '\0'};
        stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 184, 7, hotkeyString,
                                 cblack, click ? cgreen : hover ? cdarkgray : cgray);
    }
    uint32_t borderColor =
            this->setGreenTool != Tool::NOTHING ? this->setGreenTool == data->currentTool ? 0xFF00FF00 : 0xFF000000 : 0xFF000000;
    if (borderColor == 0xFF000000 && setGreenShape != Shape::DEFAULT) {
        borderColor = setGreenShape == data->currentShape ? 0xFF00FF00 : 0xFF000000;
    }
    renderBorder(borderColor);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, xStringpos, 7, info,
                             cblack, click ? cgreen : hover ? cdarkgray : cgray);
}

/**
 * \brief Renders the value of the Button.
 *
 * \param text The text to display on the Button.
 */
void Button::renderValue(const char *text) {
    renderBackground(0, 40, mouseX < 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(160, 200, mouseX > 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(40, 160, mouseX >= 40 && mouseX <= 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(0xFF000000);
    lineDrawer->drawLine(39, 0, 39, 30, cblack);
    lineDrawer->drawLine(40, 0, 40, 30, cblack);
    lineDrawer->drawLine(160, 0, 160, 30, cblack);
    lineDrawer->drawLine(161, 0, 161, 30, cblack);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 100 - (strlen(text) * 4), 7, text, cblack,
                             mouseX >= 40 && mouseX <= 160 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Util::Graphic::Fonts::SUN_12x22, 14, 4, "-", cred,
                             mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Util::Graphic::Fonts::SUN_12x22, 174, 4, "+", cgreen,
                             mouseX > 160 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
}

/**
 * \brief Renders the Button when it is of type INT_VALUE.
 */
void Button::renderIntValue() {
    const char *int_string = int_to_string(*intValue);
    char text[256];
    snprintf(text, sizeof(text), "%s: %s", info, int_string);
    renderValue(text);
}

/**
 * \brief Renders the Button when it is of type DOUBLE_VALUE.
 */
void Button::renderDoubleValue() {
    const char *double_string = double_to_string(*doubleValue, 2);
    char text[256];
    snprintf(text, sizeof(text), "%s: %s", info, double_string);
    renderValue(text);
}

/**
 * \brief Renders the Button when it is of type BOOLEAN.
 */
void Button::renderBoolean() {
    renderBackground(0, 200, click ? green : hover ? darkgray : gray);
    renderBorder(*boolValue ? 0xFF00FF00 : 0xFFFF0000);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 100 - (strlen(info) * 4), 7, info,
                             cblack, click ? cgreen : hover ? cdarkgray : cgray);
}

/**
 * \brief Renders the Button when it is of type CONFIRM.
 */
void Button::renderConfirm() {
    renderBackground(0, 100, mouseX < 100 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(100, 200, mouseX >= 100 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(0xFF000000);
    lineDrawer->drawLine(99, 0, 99, 30, cblack);
    lineDrawer->drawLine(100, 0, 100, 30, cblack);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 50 - (strlen("RESET") * 4), 7, "RESET", cred,
                             mouseX < 100 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 150 - (strlen("OK") * 4), 7, "OK", cgreen,
                             mouseX >= 100 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
}

/**
 * \brief Renders the Button when it is of type LAYER.
 */
void Button::renderLayer() {
    if (layerNum > data->layers->countLayersNum() - 1) {
        for (int i = 0; i < 30 * 200; i++) {
            this->buffer[i] = 0x00000000;
        }
        return;
    }
    renderBackground(0, 40, mouseX < 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(40, 80, mouseX >= 40 && mouseX < 80 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(80, 120, mouseX >= 80 && mouseX < 120 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(120, 160, mouseX >= 120 && mouseX < 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(160, 200, mouseX >= 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(0xFF000000);
    lineDrawer->drawLine(39, 0, 39, 30, cblack);
    lineDrawer->drawLine(40, 0, 40, 30, cblack);
    lineDrawer->drawLine(79, 0, 79, 30, cblack);
    lineDrawer->drawLine(80, 0, 80, 30, cblack);
    lineDrawer->drawLine(119, 0, 119, 30, cblack);
    lineDrawer->drawLine(120, 0, 120, 30, cblack);
    lineDrawer->drawLine(159, 0, 159, 30, cblack);
    lineDrawer->drawLine(160, 0, 160, 30, cblack);
    const char *num = int_to_string(layerNum);
    int xStringpos = 20 - (strlen(num) * 4);
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, xStringpos, 7, num,
                             data->layers->currentLayerNum() == layerNum ? cgreen : cblack,
                             mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawMonoBitmap(52, 7, 16, 16, cblack, mouseX >= 40 && mouseX < 80 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::arrow_up);
    stringDrawer->drawMonoBitmap(92, 7, 16, 16, cblack, mouseX >= 80 && mouseX < 120 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::arrow_down);
    stringDrawer->drawMonoBitmap(132, 7, 16, 16, cblack, mouseX >= 120 && mouseX < 160 ?
                                                         (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::trashcan);
    Util::Graphic::Color eyeFG = data->layers->at(layerNum)->isVisible ? cgreen : cblack;
    stringDrawer->drawMonoBitmap(172, 7, 16, 16, eyeFG, mouseX >= 160 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::eye);
}

/**
 * \brief Renders the Button when it is of type INPUT.
 */
void Button::renderInput() {
    renderBackground(0, 40, mouseX < 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(40, 200, mouseX >= 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(*captureInput ? 0xFF00FF00 : 0xFF000000);
    lineDrawer->drawLine(39, 0, 39, 30, *captureInput ? cgreen : cblack);
    lineDrawer->drawLine(40, 0, 40, 30, *captureInput ? cgreen : cblack);
    if (bitmap != nullptr) {
        stringDrawer->drawMonoBitmap(12, 7, 16, 16, cred, mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray, bitmap);
    } else {
        stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 8, 7, "DEL", cred,
                                 mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    }
    stringDrawer->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 48, 7, input->operator const char *(), cblack,
                             mouseX >= 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
}

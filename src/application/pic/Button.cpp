//
// Created by Rafael Reip on 06.10.24.
//

#include "Button.h"


Button::Button(DataWrapper *data) {
    this->data = data;
    this->buffer = new uint32_t[30 * 200];
    for (int i = 0; i < 30 * 200; i++) {
        this->buffer[i] = 0x00000000;
    }
    this->lfb = new LinearFrameBuffer(this->buffer, 200, 30, 32, 200 * 4, false);
    this->pixelDrawer = new PixelDrawer(*lfb);
    this->lineDrawer = new LineDrawer(*pixelDrawer);
    this->stringDrawer = new StringDrawer(*pixelDrawer);
    this->bufferChanged = true;
    this->click = false;
    this->hover = false;
    this->type = NONE;
    this->info = "Button";
    this->hotkey = 0;
    this->hasHotkey = false;
    this->cblack = Color(0, 0, 0);
    this->cgray = Color(128, 128, 128);
    this->cdarkgray = Color(80, 80, 80);
    this->cgreen = Color(0, 255, 0);
    this->cred = Color(255, 0, 0);
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

Button *Button::setHotkey(char h) {
    this->hotkey = h;
    return this;
}

Button *Button::set16Bitmap(uint8_t *b) {
    this->bitmap = b;
    return this;
}

Button *Button::setInfo(const char *string) {
    this->info = string;
    return this;
}

Button *Button::setMethodButton(void (*method)(DataWrapper *)) {
    this->type = METHOD;
    this->method1 = method;
    render();
    return this;
}

Button *Button::setRenderFlagMethod(void (RenderFlags::*m)()) {
    this->rFlagMethod = m;
    return this;
}

Button *Button::setIntValueButton(int *value) {
    this->type = INT_VALUE;
    this->intValue = value;
    render();
    return this;
}

Button *Button::setIntValueButton(int *value, int limit_low, int limit_high) {
    this->type = INT_VALUE;
    this->intValue = value;
    this->intLimitLow = limit_low;
    this->intLimitHigh = limit_high;
    this->hasIntLimits = true;
    render();
    return this;
}

Button *Button::setDoubleValueButton(double *dvalue) {
    this->type = DOUBLE_VALUE;
    this->doubleValue = dvalue;
    render();
    return this;
}

Button *Button::setDoubleValueButton(double *dvalue, double limit_low, double limit_high) {
    this->type = DOUBLE_VALUE;
    this->doubleValue = dvalue;
    this->doubleLimitLow = limit_low;
    this->doubleLimitHigh = limit_high;
    this->hasDoubleLimits = true;
    render();
    return this;
}

Button *Button::setInputButton(Util::String *s, bool *capture) {
    this->type = INPUT;
    this->input = s;
    this->captureInput = capture;
    render();
    return this;
}

Button *Button::setConfirmButton(void (*cancel)(DataWrapper *), void (*ok)(DataWrapper *)) {
    this->type = CONFIRM;
    this->method1 = cancel;
    this->method2 = ok;
    render();
    return this;
}

Button *Button::setLayerButton(int num) {
    this->type = LAYER;
    this->layerNum = num;
    render();
    return this;
}

Button *Button::changeGreenIfTool(Tool tool) {
    this->setGreenTool = tool;
    render();
    return this;
}

Button *Button::changeGreenIfShape(Shape shape) {
    setGreenShape = shape;
    render();
    return this;
}

Button *Button::setColor(int *a, int *r, int *g, int *b) {
    colorA = a, colorR = r, colorG = g, colorB = b;
    showcolor = true;
    render();
    return this;
}

Button *Button::setAppearTopOnChange(bool set) {
    this->appearTopOnChange = set;
    return this;
}

Button *Button::setAppearBottomOnChange(bool set) {
    this->appearBottomOnChange = set;
    return this;
}

uint32_t *Button::getBuffer() {
    return this->buffer;
}

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
                } else {
                    *doubleValue = 0.01;
                }
            }
            if (hasDoubleLimits) {
                if (*doubleValue > doubleLimitHigh) *doubleValue = doubleLimitHigh;
                if (*doubleValue < doubleLimitLow) *doubleValue = doubleLimitLow;
            }
            break;
        case CONFIRM:
            if (relX < 100) {
                method1(data);
            } else {
                method2(data);
            }
            break;
        case LAYER:
            if (layerNum > data->layers->countNum()) return;
            if (relX < 40) {
                data->layers->setCurrent(layerNum);
            } else if (relX < 80) {
                data->layers->swap(layerNum, layerNum - 1);
                if (layerNum == data->layers->currentNum()) data->layers->setCurrent(layerNum - 1);
            } else if (relX < 120) {
                data->layers->swap(layerNum, layerNum + 1);
                if (layerNum == data->layers->currentNum()) data->layers->setCurrent(layerNum + 1);
            } else if (relX < 160) {
                data->layers->deletetAt(layerNum);
            } else {
                data->layers->at(layerNum)->isVisible = !data->layers->at(layerNum)->isVisible;
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
    if (appearTopOnChange) {
        data->currentGuiLayer->appear();
    }
    if (appearBottomOnChange) {
        data->currentGuiLayerBottom->appear();
    }
    render();
}

void Button::showClick(int relX, int relY) {
    this->mouseX = relX;
    this->mouseY = relY;
    click = true;
    hover = false;
    render();
}

void Button::showHover(int relX, int relY) {
    this->mouseX = relX;
    this->mouseY = relY;
    hover = true;
    click = false;
    render();
}

void Button::removeInteraction() {
    hover = false;
    click = false;
    render();
}

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

void Button::renderBackground(int x1, int x2, uint32_t color) {
    for (int i = 0; i < 30 * 200; i += 200) {
        for (int j = i + x1; j < i + x2; j++) {
            this->buffer[j] = color;
        }
    }
}

void Button::renderMethod() {
    renderBackground(0, 200, click ? green : hover ? darkgray : gray);
    int xStringpos = 100 - (strlen(info) * 4);
    if (showcolor) {
//        uint32_t color = (*colorA << 24) | (*colorR << 16) | (*colorG << 8) | *colorB;
        uint32_t color = 0xFF000000 | (*colorR << 16) | (*colorG << 8) | *colorB;
        renderBackground(0, 80, color);
        Color c = this->setGreenTool == data->currentTool ? cgreen : cblack;
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
    uint32_t borderColor = this->setGreenTool != Tool::NOTHING ? this->setGreenTool == data->currentTool ? 0xFF00FF00 : 0xFF000000 : 0xFF000000;
    if(borderColor == 0xFF000000 && setGreenShape != Shape::DEFAULT) {
        borderColor = setGreenShape == data->currentShape ? 0xFF00FF00 : 0xFF000000;
    }
    renderBorder(borderColor);
    stringDrawer->drawString(Fonts::TERMINAL_8x16, xStringpos, 7, info, cblack,
                             click ? cgreen : hover ? cdarkgray : cgray);
}

void Button::renderValue(const char *text) {
    renderBackground(0, 40, mouseX < 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(160, 200, mouseX > 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(40, 160, mouseX >= 40 && mouseX <= 160 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(0xFF000000);
    lineDrawer->drawLine(39, 0, 39, 30, cblack);
    lineDrawer->drawLine(40, 0, 40, 30, cblack);
    lineDrawer->drawLine(160, 0, 160, 30, cblack);
    lineDrawer->drawLine(161, 0, 161, 30, cblack);
    stringDrawer->drawString(Fonts::TERMINAL_8x16, 100 - (strlen(text) * 4), 7, text, cblack,
                             mouseX >= 40 && mouseX <= 160 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Fonts::SUN_12x22, 14, 4, "-", cred,
                             mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Fonts::SUN_12x22, 174, 4, "+", cgreen,
                             mouseX > 160 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
}

void Button::renderIntValue() {
    const char *int_string = int_to_string(*intValue);
    char text[256];
    snprintf(text, sizeof(text), "%s: %s", info, int_string);
    renderValue(text);
}

void Button::renderDoubleValue() {
    const char *double_string = double_to_string(*doubleValue, 2);
    char text[256];
    snprintf(text, sizeof(text), "%s: %s", info, double_string);
    renderValue(text);
}

void Button::renderConfirm() {
    renderBackground(0, 100, mouseX < 100 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(100, 200, mouseX >= 100 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(0xFF000000);
    lineDrawer->drawLine(99, 0, 99, 30, cblack);
    lineDrawer->drawLine(100, 0, 100, 30, cblack);
    stringDrawer->drawString(Fonts::TERMINAL_8x16, 50 - (strlen("RESET") * 4), 7, "RESET", cred,
                             mouseX < 100 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawString(Fonts::TERMINAL_8x16, 150 - (strlen("OK") * 4), 7, "OK", cgreen,
                             mouseX >= 100 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);

}

void Button::renderLayer() {
    if (layerNum > data->layers->countNum() - 1) {
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
    stringDrawer->drawString(Fonts::TERMINAL_8x16, xStringpos, 7, num, data->layers->currentNum() == layerNum ? cgreen : cblack,
                             mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    stringDrawer->drawMonoBitmap(52, 7, 16, 16, cblack, mouseX >= 40 && mouseX < 80 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::arrow_up);
    stringDrawer->drawMonoBitmap(92, 7, 16, 16, cblack, mouseX >= 80 && mouseX < 120 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::arrow_down);
    stringDrawer->drawMonoBitmap(132, 7, 16, 16, cblack, mouseX >= 120 && mouseX < 160 ?
                                                         (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::trashcan);
    Color eyeFG = data->layers->at(layerNum)->isVisible ? cgreen : cblack;
    stringDrawer->drawMonoBitmap(172, 7, 16, 16, eyeFG, mouseX >= 160 ?
                                                        (click ? cgreen : hover ? cdarkgray : cgray) : cgray, Bitmaps::eye);
}

void Button::renderInput() {
    renderBackground(0, 40, mouseX < 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBackground(40, 200, mouseX >= 40 ? (click ? green : hover ? darkgray : gray) : gray);
    renderBorder(*captureInput ? 0xFF00FF00 : 0xFF000000);
    lineDrawer->drawLine(39, 0, 39, 30, *captureInput ? cgreen : cblack);
    lineDrawer->drawLine(40, 0, 40, 30, *captureInput ? cgreen : cblack);
    if (bitmap != nullptr) {
        stringDrawer->drawMonoBitmap(12, 7, 16, 16, cred, mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray, bitmap);
    } else {
        stringDrawer->drawString(Fonts::TERMINAL_8x16, 8, 7, "DEL", cred,
                                 mouseX < 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);
    }
    stringDrawer->drawString(Fonts::TERMINAL_8x16, 48, 7, input->operator const char *(), cblack,
                             mouseX >= 40 ? (click ? cgreen : hover ? cdarkgray : cgray) : cgray);

}

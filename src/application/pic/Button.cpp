//
// Created by Rafael Reip on 06.10.24.
//

#include "Button.h"


Button::Button(void (*clickMethod)(UiData *uiData), UiData *uiData) : clickMethod(clickMethod), uiData(uiData) {
    this->buffer = new uint32_t[30 * 200];
    this->bufferChanged = true;
    this->click = false;
    this->hover = false;
    render();
}

void Button::processClick(int relX, int relY) {
    clickMethod(uiData);
    render();
}

void Button::showClick(int relX, int relY) {
    click = true;
    hover = false;
    render();
}

void Button::showHover(int relX, int relY) {
    hover = true;
    click = false;
    render();
}

void Button::removeInteraction() {
    hover = false;
    click = false;
    render();
}

uint32_t *Button::getBuffer() {
    return this->buffer;
}

void Button::render() {
    for (int i = 0; i < 30 * 200; i++) {
        this->buffer[i] = click ? 0xFFFF0000 : hover ? 0xFF00FF00 : 0xFF0000FF;
    }
    int border = 2;
    for (int i = 0; i < border * 200; i++) { // top border
        this->buffer[i] = 0xFF000000;
    }
    for (int i = (30 - border) * 200; i < 30 * 200; i++) { // bottom border
        this->buffer[i] = 0xFF000000;
    }
    for (int i = 0; i < 30 * 200; i += 200) { // left and right border
        for (int j = 0; j < border; j++) {
            this->buffer[i + j] = 0xFF000000;
            this->buffer[i + 200 - j - 1] = 0xFF000000;
        }
    }
    this->bufferChanged = true;
}

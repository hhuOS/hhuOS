//
// Created by Rafael Reip on 04.10.24.
//

#include "Renderer.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush


Renderer::Renderer(DataWrapper *data) {
    this->data = data;
    this->buff_lfb = reinterpret_cast<uint32_t *>(data->lfb->getBuffer().get());
    this->buff_result = newBuffer(data->screenAll);
    this->buff_base = newBuffer(data->screenAll);
    this->buff_workarea = newBuffer(data->workAreaAll);
    this->buff_gui = newBuffer(data->guiAll);
    this->buff_overlay = newBuffer(data->workAreaAll);
    this->buff_layers = newBuffer(data->workAreaAll);
    this->buff_under_current = newBuffer(data->workAreaAll);
    this->buff_over_current = newBuffer(data->workAreaAll);
    prepareBase();
}

uint32_t *Renderer::newBuffer(int size) {
    auto *buff = new uint32_t[size];
    for (int i = 0; i < size; i++) {
        buff[i] = 0;
    }
    return buff;
}


void Renderer::prepareBase() {
    const int squareSize = 10;
    const uint32_t lightGray = 0xFFC0C0C0, darkGray = 0xFF404040, Gray = 0xFF808080;
    int offset = 0;
    for (int y = 0; y < data->screenY; y++) {
        for (int x = 0; x < 200; x++) { // Gray for GUI
            buff_base[offset++] = Gray;
        }
        bool light = (y / squareSize) % 2;
        for (int x = 200; x < data->screenX;) { // Alternating squares for Background
            for (int k = 0; k < 10 && x < data->screenX; k++, x++) {
                buff_base[offset++] = light ? lightGray : darkGray;
            }
            light = !light;
        }
    }
}

// TODO: alles durchgehen, ob in keiner Situation zu viel gerendert wird
//void Renderer::run() {
void Renderer::render() {
//    while (true) {
        if (data->flags->anyChange) {
            if (data->flags->result) {
                renderResult();
                for (int i = 0; i < data->screenAll; i++) {
                    buff_lfb[i] = buff_result[i];
                }
            } else {
                removeMouse();
            }
            renderMouse();
            data->flags->mouse = false;
            data->flags->anyChange = false;
        }
//        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
//    }
}


void Renderer::renderMouse() {
    int mouseX = data->mouseX;
    int mouseY = data->mouseY;

    for (int y = mouseY; y < mouseY + 10 && y < data->screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < data->screenX; x++) {
            int offset = y * data->screenX + x;
            buff_lfb[offset] = data->leftButtonPressed ? 0xFFFF0000 : 0xFF00FF00;
        }
    }
}

void Renderer::removeMouse() {
    int mouseX = data->oldMouseX;
    int mouseY = data->oldMouseY;

    for (int y = mouseY; y < mouseY + 10 && y < data->screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < data->screenX; x++) {
            int offset = y * data->screenX + x;
            buff_lfb[offset] = buff_result[offset];
        }
    }
}

// TODO: jeweils nur rechte oder linke Seite neu rendern, wenn nur 1 flag gesetzt
void Renderer::renderResult() {
    if (data->flags->workArea) {
        renderWorkArea();
    }
    for (int i = 0; i < data->screenAll; i++) {
        buff_result[i] = buff_base[i];
    }
    if (data->flags->gui) {
        renderGui();
    }

    blendBuffers(buff_result, buff_workarea, data->screenX, data->screenY, data->workAreaX, data->workAreaY, 200, 0);
    blendBuffers(buff_result, buff_gui, data->screenX, data->screenY, 200, data->screenY, 0, 0);
    data->flags->result = false;
}

void Renderer::renderWorkArea() {
    if (data->flags->layers) {
        renderLayers();
    }
    if (data->flags->overlay) {
        renderOverlay();
    }
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_workarea[i] = buff_layers[i];
    }
    blendBuffers(buff_workarea, buff_overlay, data->workAreaAll);
    data->flags->workArea = false;
}

// TODO: flags neu machen
void Renderer::renderGui() {
    for (int i = 0; i < data->guiX * data->screenY; i++) {
        buff_gui[i] = 0x00000000;
    }
    auto guiLayer = data->currentGuiLayer;
    for (int i = 0; i < guiLayer->buttonCount; ++i) {
        if (guiLayer->buttons[i]->bufferChanged) {
            blendBuffers(buff_gui, guiLayer->buttons[i]->getBuffer(), data->guiX, data->screenY, 200, 30, 0, i * 30);
            guiLayer->buttons[i]->bufferChanged = false;
        }
    }
    data->flags->gui = false;
}

void Renderer::renderOverlay() {
    for (int i = data->workAreaAll - 10 * data->workAreaX; i < data->workAreaAll; i++) {
        buff_overlay[i] = 0x80FF0000;
    }
    data->flags->overlay = false;
}

void Renderer::renderLayers() {
    if (data->flags->layerOrder) {
        for (int i = 0; i < data->workAreaAll; i++) {
            buff_over_current[i] = 0x00000000;
            buff_under_current[i] = 0x00000000;
        }
        for (int i = 0; i < data->currentLayer; i++) { // für buff_under_current
            auto layer = data->layers[i];
            if (layer->getVisibility())
                blendBuffers(buff_under_current, layer->getPixelData(), data->workAreaX, data->workAreaY,
                             layer->getWidth(),
                             layer->getHeight(), layer->getPosX(), layer->getPosY());
        }
        for (int i = data->currentLayer + 1; i < data->layerCount; i++) { // für buff_over_current
            auto layer = data->layers[i];
            if (layer->getVisibility())
                blendBuffers(buff_over_current, layer->getPixelData(), data->workAreaX, data->workAreaY,
                             layer->getWidth(),
                             layer->getHeight(), layer->getPosX(), layer->getPosY());
        }
    }
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_layers[i] = 0x00000000;
    }
    auto currentLayer = data->layers[data->currentLayer];
    blendBuffers(buff_layers, buff_under_current, data->workAreaAll);
    if (currentLayer->getVisibility())
        blendBuffers(buff_layers, currentLayer->getPixelData(), data->workAreaX, data->workAreaY,
                     currentLayer->getWidth(),
                     currentLayer->getHeight(), currentLayer->getPosX(), currentLayer->getPosY());
    blendBuffers(buff_layers, buff_over_current, data->workAreaAll);
    data->flags->layers = false;
}

uint32_t Renderer::blendPixels(uint32_t lower, uint32_t upper) {
    uint8_t upperAlpha = (upper >> 24) & 0xFF;
    uint8_t lowerAlpha = (lower >> 24) & 0xFF;
    if (upperAlpha == 0xFF) return upper;
    else if (upperAlpha == 0) return lower;
    uint8_t inverseAlpha = 255 - upperAlpha;
    uint8_t r = ((upper >> 16) & 0xFF) * upperAlpha / 255 + ((lower >> 16) & 0xFF) * inverseAlpha / 255;
    uint8_t g = ((upper >> 8) & 0xFF) * upperAlpha / 255 + ((lower >> 8) & 0xFF) * inverseAlpha / 255;
    uint8_t b = (upper & 0xFF) * upperAlpha / 255 + (lower & 0xFF) * inverseAlpha / 255;
    uint8_t a = upperAlpha + (lowerAlpha * inverseAlpha) / 255;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

void Renderer::blendBuffers(uint32_t *lower, const uint32_t *upper, int size) {
    for (int i = 0; i < size; i++) {
        lower[i] = blendPixels(lower[i], upper[i]);
    }
}

void Renderer::blendBuffers(uint32_t *lower, const uint32_t *upper, int lx, int ly, int ux, int uy, int px, int py) {
    for (int y = 0; y < uy; y++) {
        int lowerY = y + py;
        if (lowerY < 0 || lowerY >= ly) continue;
        for (int x = 0; x < ux; x++) {
            int lowerX = x + px;
            if (lowerX < 0 || lowerX >= lx) continue;
            int upperIndex = y * ux + x;
            int lowerIndex = lowerY * lx + lowerX;
            lower[lowerIndex] = blendPixels(lower[lowerIndex], upper[upperIndex]);
        }
    }
}


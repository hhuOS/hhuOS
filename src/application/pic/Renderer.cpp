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
    this->lastRenderedMouseX = 0;
    this->lastRenderedMouseY = 0;
    this->lfb = new LinearFrameBuffer(this->buff_overlay, data->workAreaX, data->workAreaY, 32, data->workAreaX * 4, false);
    this->pixelDrawer = new PixelDrawer(*lfb);
    this->lineDrawer = new LineDrawer(*pixelDrawer);
    this->stringDrawer = new StringDrawer(*pixelDrawer);
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
    lastRenderedMouseX = data->mouseX;
    lastRenderedMouseY = data->mouseY;
    int mouseX = data->mouseX;
    int mouseY = data->mouseY;

    for (int y = mouseY; y < mouseY + 10 && y < data->screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < data->screenX; x++) {
            int offset = y * data->screenX + x;
            if (y == mouseY || y == mouseY + 9 || x == mouseX || x == mouseX + 9) {
                buff_lfb[offset] = 0xFF000000;
            } else {
                buff_lfb[offset] = data->leftButtonPressed ? 0xFFFF0000 : 0xFF00FF00;
            }
        }
    }
}

void Renderer::removeMouse() {
    int mouseX = lastRenderedMouseX;
    int mouseY = lastRenderedMouseY;

    for (int y = mouseY; y < mouseY + 10 && y < data->screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < data->screenX; x++) {
            int offset = y * data->screenX + x;
            buff_lfb[offset] = buff_result[offset];
        }
    }
}

void Renderer::renderResult() {
    for (int i = 0; i < data->screenAll;) {
        if (data->flags->gui) {
            for (int j = 0; j < data->guiX; j++) {
                buff_result[i++] = buff_base[i];
            }
        } else i += data->guiX;
        if (data->flags->workArea) {
            for (int j = 0; j < data->workAreaX; j++) {
                buff_result[i++] = buff_base[i];
            }
        } else i += data->workAreaX;
    }
    if (data->flags->workArea) {
        renderWorkArea();
        blendBuffers(buff_result, buff_workarea, data->screenX, data->screenY, data->workAreaX, data->workAreaY, 200, 0);
    }
    if (data->flags->gui) {
        renderGui();
        blendBuffers(buff_result, buff_gui, data->screenX, data->screenY, 200, data->screenY, 0, 0);
    }
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

void Renderer::renderGui() {
    auto guiLayer = data->currentGuiLayer;
    auto guiLayerBottom = data->currentGuiLayerBottom;

    if (data->flags->guiLayer) for (int i = 0; i < data->guiX * data->screenY; i++) buff_gui[i] = 0x00000000;

    for (int i = 0; i < guiLayer->buttonCount; i++) { // top buttons
        if (data->flags->guiLayer || guiLayer->buttons[i]->bufferChanged) {
            for (int j = i * 30 * 200; j < (i + 1) * 30 * 200; j++) buff_gui[j] = 0x00000000;
            blendBuffers(buff_gui, guiLayer->buttons[i]->getBuffer(), data->guiX, data->screenY, 200, 30, 0, i * 30);
            guiLayer->buttons[i]->bufferChanged = false;
        }
    }
    int b = 19 - guiLayerBottom->buttonCount;
    for (int i = b; i <= 18; i++) { // bottom buttons
        int x = i - b;
        if (data->flags->guiLayer || guiLayerBottom->buttons[x]->bufferChanged) {
            for (int j = i * 30 * 200; j < (i + 1) * 30 * 200; j++) buff_gui[j] = 0x00000000;
            blendBuffers(buff_gui, guiLayerBottom->buttons[x]->getBuffer(), data->guiX, data->screenY, 200, 30, 0,
                         i * 30);
            guiLayerBottom->buttons[x]->bufferChanged = false;
        }
    }
    if (data->flags->guiLayer || data->textButton->bufferChanged) { // text button
        for (int i = (data->guiY - 30) * 200; i < data->guiY * 200; i++) buff_gui[i] = 0x00000000;
        blendBuffers(buff_gui, data->textButton->getBuffer(), data->guiX, data->screenY, 200, 30, 0, data->guiY - 30);
        data->textButton->bufferChanged = false;
    }

    data->flags->gui = false;
    data->flags->guiLayer = false;
    data->flags->guiJustButton = false;
}

void Renderer::drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color color) {
    lineDrawer->drawLine(x1, y1, x2, y2, color);
    lineDrawer->drawLine(x2, y2, x3, y3, color);
    lineDrawer->drawLine(x3, y3, x4, y4, color);
    lineDrawer->drawLine(x4, y4, x1, y1, color);
}

void Renderer::renderOverlay() {
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_overlay[i] = 0x00000000;
    }
    // border for current layer
    auto l = data->layers[data->currentLayer];
    int x = l->posX, y = l->posY, w = l->width, h = l->height;
    drawOverlayBox(x - 1, y - 1, x + w, y - 1, x + w, y + h, x - 1, y + h, Color(255, 0, 0));
    drawOverlayBox(x - 2, y - 2, x + w + 1, y - 2, x + w + 1, y + h + 1, x - 2, y + h + 1, Color(255, 0, 0));

    switch (data->currentTool) {
        case Tool::MOVE:
            x = data->moveX, y = data->moveY;
            drawOverlayBox(x - 1, y - 1, x + w, y - 1, x + w, y + h, x - 1, y + h, Color(0, 255, 0));
            drawOverlayBox(x - 2, y - 2, x + w + 1, y - 2, x + w + 1, y + h + 1, x - 2, y + h + 1, Color(0, 255, 0));
            break;
    }

    if (data->debugString != nullptr) {
        stringDrawer->drawString(Fonts::TERMINAL_8x16, 0, 0, data->debugString, Color(0, 0, 0), Color(255, 255, 255));
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
            if (layer->isVisible)
                blendBuffers(buff_under_current, layer->getPixelData(), data->workAreaX, data->workAreaY,
                             layer->width,
                             layer->height, layer->posX, layer->posY);
        }
        for (int i = data->currentLayer + 1; i < data->layerCount; i++) { // für buff_over_current
            auto layer = data->layers[i];
            if (layer->isVisible)
                blendBuffers(buff_over_current, layer->getPixelData(), data->workAreaX, data->workAreaY,
                             layer->width,
                             layer->height, layer->posX, layer->posY);
        }
    }
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_layers[i] = 0x00000000;
    }
    auto currentLayer = data->layers[data->currentLayer];
    blendBuffers(buff_layers, buff_under_current, data->workAreaAll);
    if (currentLayer->isVisible)
        blendBuffers(buff_layers, currentLayer->getPixelData(), data->workAreaX, data->workAreaY,
                     currentLayer->width,
                     currentLayer->height, currentLayer->posX, currentLayer->posY);
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

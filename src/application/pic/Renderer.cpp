//
// Created by Rafael Reip on 04.10.24.
//

#include "Renderer.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush


Renderer::Renderer(RenderData *renderData, Util::Graphic::LinearFrameBuffer *lfb) {
    this->rData = renderData;
    this->lfb = lfb;
    this->buff_lfb = reinterpret_cast<uint32_t *>(lfb->getBuffer().get());
    this->screenX = lfb->getResolutionX();
    this->screenY = lfb->getResolutionY();
    this->workAreaX = screenX - 200;
    this->workAreaY = screenY;
    this->lastMouseX = -1;
    this->lastMouseY = -1;
    this->pitch = lfb->getPitch();
    this->buff_result = newBuffer(screenX * screenY);
    this->buff_base = newBuffer(screenX * screenY);
    this->buff_workarea = newBuffer(workAreaX * workAreaY);
    this->buff_gui = newBuffer(200 * screenY);
    this->buff_overlay = newBuffer(workAreaX * workAreaY);
    this->buff_layers = newBuffer(workAreaX * workAreaY);
    this->buff_under_current = newBuffer(workAreaX * workAreaY);
    this->buff_over_current = newBuffer(workAreaX * workAreaY);
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
    for (int y = 0; y < screenY; y++) {
        for (int x = 0; x < 200; x++) { // Gray for GUI
            buff_base[offset++] = Gray;
        }
        bool light = (y / squareSize) % 2;
        for (int x = 200; x < screenX;) { // Alternating squares for Background
            for (int k = 0; k < 10 && x < screenX; k++, x++) {
                buff_base[offset++] = light ? lightGray : darkGray;
            }
            light = !light;
        }
    }
}


void Renderer::run() {

    for (int i = 0; i < screenX * screenY; i++) {
        buff_lfb[i] = buff_base[i];
        buff_result[i] = buff_base[i];
    }

    while (true) {
        if (rData->flags->anyChange) {
            if (rData->flags->result) {
                renderResult();
                for (int i = 0; i < screenX * screenY; i++) {
                    buff_lfb[i] = buff_result[i];
                }
            } else {
                removeMouse();
            }
            renderMouse();
            lastMouseX = rData->mouse->X;
            lastMouseY = rData->mouse->Y;
            rData->flags->mouse = false;
            rData->flags->anyChange = false;
        }
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    while (true) { // old

        int offset = 0;
        for (int j = 0; j < screenY; j++) {
            for (int i = 0; i < screenX; i++, offset++) {
                reinterpret_cast<uint32_t *>(buff_lfb)[offset] = buff_base[offset];

                for (int k = 0; k < rData->layerCount; k++) {
                    Layer *item = rData->layers[k];
                    uint32_t *data = item->getPixelData();

                    int relX = i - 200 - item->getPosX();
                    int relY = item->getHeight() - 1 - (j - item->getPosY());
                    if (relX < 0 || relY < 0 || relX >= item->getWidth() || relY >= item->getHeight()) {
                        continue;
                    }

                    reinterpret_cast<uint32_t *>(buff_lfb)[offset] = data[relX + relY * item->getWidth()];

                }

                if (abs(i - rData->mouse->X) < 10 && abs(j - rData->mouse->Y) < 10) {
                    reinterpret_cast<uint32_t *>(buff_lfb)[offset] = rData->mouse->leftButtonPressed ? 0xFFFF0000
                                                                                                     : 0xFF00FF00;
                }
            }
        }
//        lfb->flush();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

}


void Renderer::renderMouse() {
    int mouseX = rData->mouse->X;
    int mouseY = rData->mouse->Y;

    for (int y = mouseY; y < mouseY + 10 && y < screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < screenX; x++) {
            int offset = y * screenX + x;
            buff_lfb[offset] = rData->mouse->leftButtonPressed ? 0xFFFF0000 : 0xFF00FF00;
        }
    }
}

void Renderer::removeMouse() {
    int mouseX = lastMouseX;
    int mouseY = lastMouseY;

    for (int y = mouseY; y < mouseY + 10 && y < screenY; y++) {
        for (int x = mouseX; x < mouseX + 10 && x < screenX; x++) {
            int offset = y * screenX + x;
            buff_lfb[offset] = buff_result[offset];
        }
    }
}

void Renderer::renderResult() {
    if (rData->flags->workArea) {
        renderWorkArea();
    }
    for (int i = 0; i < screenX * screenY; i++) {
        buff_result[i] = buff_base[i];
    }
    blendBuffers(buff_result, buff_workarea, screenX, screenY, workAreaX, workAreaY, 200, 0);
    blendBuffers(buff_result, buff_gui, screenX, screenY, 200, screenY, 0, 0);
    rData->flags->result = false;
}

void Renderer::renderWorkArea() {
    if (rData->flags->layers) {
        renderLayers();
    }
    if (rData->flags->overlay) {
        renderOverlay();
    }
    for (int i = 0; i < workAreaX * workAreaY; i++) {
        buff_workarea[i] = buff_layers[i];
    }
    blendBuffers(buff_workarea, buff_overlay, workAreaX * workAreaY);
    rData->flags->workArea = false;
}

void Renderer::renderOverlay() {
    for (int i = workAreaX * workAreaY - 10 * workAreaX; i < workAreaX * workAreaY; i++) {
        buff_overlay[i] = 0x80FF0000;
    }
    rData->flags->overlay = false;
}

void Renderer::renderLayers() {
    for (int i = 0; i < workAreaX * workAreaY; i++) {
        buff_layers[i] = 0x8000FF00;
    }
    rData->flags->layers = false;
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
        for (int x = 0; x < ux; x++) {
            int upperIndex = y * ux + x;
            int lowerIndex = (y + py) * lx + (x + px);
            if (x + px < lx && y + py < ly) {
                lower[lowerIndex] = blendPixels(lower[lowerIndex], upper[upperIndex]);
            }
        }
    }
}


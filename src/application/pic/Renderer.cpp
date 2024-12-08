//
// Created by Rafael Reip on 04.10.24.
//

#include "Renderer.h"

#include "string.h"
#include "lib/libc/stdlib.h"
#include "lib/libc/math.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/LineDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/graphic/font/Terminal8x16.h"
#include "lib/util/base/Address.h"

#include "DataWrapper.h"
#include "helper.h"
#include "Button.h"
#include "GuiLayer.h"
#include "Layer.h"
#include "Settings.h"
#include "Layers.h"
#include "MessageHandler.h"


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
    this->lfb_overlay = new Util::Graphic::LinearFrameBuffer(this->buff_overlay, data->workAreaX, data->workAreaY,
                                                             32, data->workAreaX * 4, false);
    this->pixelDrawer_overlay = new Util::Graphic::PixelDrawer(*lfb_overlay);
    this->lineDrawer_overlay = new Util::Graphic::LineDrawer(*pixelDrawer_overlay);
    this->stringDrawer_overlay = new Util::Graphic::StringDrawer(*pixelDrawer_overlay);
    this->pixelDrawer_lfb = new Util::Graphic::PixelDrawer(*data->lfb);
    this->stringDrawer_lfb = new Util::Graphic::StringDrawer(*pixelDrawer_lfb);
    this->pixelDrawer_blfb = new Util::Graphic::PixelDrawer(*data->blfb);
    this->stringDrawer_blfb = new Util::Graphic::StringDrawer(*pixelDrawer_blfb);
    this->cblack = Util::Graphic::Color(0, 0, 0);
    this->cwhite = Util::Graphic::Color(255, 255, 255);
    this->cgreen = Util::Graphic::Color(0, 255, 0);
    this->cred = Util::Graphic::Color(255, 0, 0);
    this->cgray = Util::Graphic::Color(128, 128, 128);
    this->lastTime = time(nullptr);
    this->usingBufferedBuffer = false;
    this->frames = 0;
    this->fps = 0;
    prepareBase();
}

Renderer::~Renderer() {
    delete[] buff_result;
    delete[] buff_base;
    delete[] buff_workarea;
    delete[] buff_gui;
    delete[] buff_overlay;
    delete[] buff_layers;
    delete[] buff_under_current;
    delete[] buff_over_current;
    delete lfb_overlay;
    delete pixelDrawer_overlay;
    delete lineDrawer_overlay;
    delete stringDrawer_overlay;
    delete pixelDrawer_lfb;
    delete stringDrawer_lfb;
    delete pixelDrawer_blfb;
    delete stringDrawer_blfb;
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
        if (data->settings->checkeredBackground) {
            bool light = (y / squareSize) % 2;
            for (int x = 200; x < data->screenX;) { // Alternating squares for Background
                for (int k = 0; k < 10 && x < data->screenX; k++, x++) {
                    buff_base[offset++] = light ? lightGray : darkGray;
                }
                light = !light;
            }
        } else {
            for (int x = 200; x < data->screenX; x++) { // No Color for Background
                buff_base[offset++] = 0x00000000;
            }
        }
    }
}

// TODO: alles durchgehen, ob in keiner Situation zu viel gerendert wird
void Renderer::render() {
    if (data->settings->useBufferedBuffer != usingBufferedBuffer) {
        usingBufferedBuffer = data->settings->useBufferedBuffer;
        if (usingBufferedBuffer) {
            buff_lfb = reinterpret_cast<uint32_t *>(data->blfb->getBuffer().get());
        } else {
            buff_lfb = reinterpret_cast<uint32_t *>(data->lfb->getBuffer().get());
        }
    }
    if (data->flags->anyChange || !data->settings->optimizeRendering) {
        if (data->flags->result || !data->settings->optimizeRendering) {
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
    if (data->settings->showFPS) {
        time_t now = time(nullptr);
        time_t delta = now - lastTime;
        if (delta >= 1) {
            fps = frames / delta;
            frames = 0;
        }
        auto fpsString = Util::String::format("FPS: %d", fps).operator const char *();
        while (strlen(fpsString) < 15) fpsString = Util::String::format("%s ", fpsString).operator const char *();
        frames++;
        if (usingBufferedBuffer) {
            stringDrawer_blfb->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 200, 0, fpsString, cblack, cwhite);
        } else {
            stringDrawer_lfb->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 200, 0, fpsString, cblack, cwhite);
        }
        lastTime = now;
    }
    if (usingBufferedBuffer) {
        data->blfb->flush();
    }
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
    if (data->flags->gui || data->flags->workArea || !data->settings->optimizeRendering) {
        if (data->flags->base || !data->settings->optimizeRendering) {
            prepareBase();
            data->flags->base = false;
        }
        for (int i = 0; i < data->screenAll;) {
            if (data->flags->gui || !data->settings->optimizeRendering) {
                for (int j = 0; j < data->guiX; j++) {
                    buff_result[i] = buff_base[i];
                    i++;
                }
            } else i += data->guiX;
            if (data->flags->workArea || !data->settings->optimizeRendering) {
                for (int j = 0; j < data->workAreaX; j++) {
                    buff_result[i] = buff_base[i];
                    i++;
                }
            } else i += data->workAreaX;
        }

    }
    if (data->flags->workArea || !data->settings->optimizeRendering) {
        renderWorkArea();
        blendBuffers(buff_result, buff_workarea, data->screenX, data->screenY, data->workAreaX, data->workAreaY, 200, 0);
    }
    if (data->flags->gui || !data->settings->optimizeRendering) {
        renderGui();
        blendBuffers(buff_result, buff_gui, data->screenX, data->screenY, 200, data->screenY, 0, 0);
    }
    data->flags->result = false;
}

void Renderer::renderWorkArea() {
    if (data->flags->layers || !data->settings->optimizeRendering) {
        renderLayers();
    }
    if (data->flags->overlay || !data->settings->optimizeRendering) {
        renderOverlay();
    }
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_workarea[i] = buff_layers[i];
    }
    blendBuffers(buff_workarea, buff_overlay, data->workAreaAll);
    uint32_t *buff_message = data->mHandler->getBuffer();
    blendBuffers(buff_workarea, buff_message, data->workAreaAll);
    data->flags->messages = false;
    data->flags->workArea = false;
}

void Renderer::renderGui() {
    auto guiLayer = data->currentGuiLayer;
    auto guiLayerBottom = data->currentGuiLayerBottom;

    if (data->flags->guiLayer || !data->settings->optimizeRendering)
        for (int i = 0; i < data->guiX * data->screenY; i++)
            buff_gui[i] = 0x00000000;

    for (int i = 0; i < guiLayer->buttonCount; i++) { // top buttons
        if (data->flags->guiLayer || guiLayer->buttons[i]->bufferChanged || !data->settings->optimizeRendering) {
            for (int j = i * 30 * 200; j < (i + 1) * 30 * 200; j++) buff_gui[j] = 0x00000000;
            blendBuffers(buff_gui, guiLayer->buttons[i]->getBuffer(), data->guiX, data->screenY, 200, 30, 0, i * 30);
            guiLayer->buttons[i]->bufferChanged = false;
        }
    }
    int b = (data->buttonCount - 1) - guiLayerBottom->buttonCount;
    for (int i = b; i < (data->buttonCount - 1); i++) { // bottom buttons
        int x = i - b;
        if (data->flags->guiLayer || guiLayerBottom->buttons[x]->bufferChanged || !data->settings->optimizeRendering) {
            for (int j = i * 30 * 200; j < (i + 1) * 30 * 200; j++) buff_gui[j] = 0x00000000;
            blendBuffers(buff_gui, guiLayerBottom->buttons[x]->getBuffer(), data->guiX, data->screenY, 200, 30, 0, i * 30);
            guiLayerBottom->buttons[x]->bufferChanged = false;
        }
    }
    if (data->flags->guiLayer || data->textButton->bufferChanged || !data->settings->optimizeRendering) { // text button
        for (int i = (data->guiY - 30) * 200; i < data->guiY * 200; i++) buff_gui[i] = 0x00000000;
        blendBuffers(buff_gui, data->textButton->getBuffer(), data->guiX, data->screenY, 200, 30, 0, data->guiY - 30);
        data->textButton->bufferChanged = false;
    }

    data->flags->gui = false;
    data->flags->guiLayer = false;
}

void Renderer::drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Util::Graphic::Color color) {
    drawOverlayBox(x1, y1, x2, y2, x3, y3, x4, y4, color, color, color, color);
}

void Renderer::drawOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,
                              Util::Graphic::Color c1, Util::Graphic::Color c2, Util::Graphic::Color c3, Util::Graphic::Color c4) {
    lineDrawer_overlay->drawLine(x1, y1, x2, y2, c1);
    lineDrawer_overlay->drawLine(x2, y2, x3, y3, c2);
    lineDrawer_overlay->drawLine(x3, y3, x4, y4, c3);
    lineDrawer_overlay->drawLine(x4, y4, x1, y1, c4);
}

void Renderer::drawFilledOverlayBox(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint32_t color) {
    for (int y = y1; y <= y3; y++) {
        for (int x = x1; x <= x2; x++) {
            buff_overlay[y * data->workAreaX + x] = color;
        }
    }
}

void Renderer::renderOverlay() {
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_overlay[i] = 0x00000000;
    }
    if (data->layers->currentNum() >= 0) {
        Layer *l = data->layers->current();
        int x = l->posX, y = l->posY, w = l->width, h = l->height;
        if (data->settings->currentLayerOverlay) {
            // border for current layer
            drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, cred);
            drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, cred);
        }
        Util::Graphic::Color top, bottom, left, right;
        if (data->currentTool == Tool::MOVE) {
            x = data->moveX, y = data->moveY;
            drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, cgreen);
            drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, cgreen);
        } else if (data->currentTool == Tool::SCALE) {
            double factor = data->scale;
            w = ceil(w * factor);
            h = ceil(h * factor);
            if (data->toolCorner == ToolCorner::TOP_LEFT ||
                data->toolCorner == ToolCorner::BOTTOM_LEFT) {
                x = floor(l->posX + l->width * (1 - factor));
            }
            if (data->toolCorner == ToolCorner::TOP_LEFT ||
                data->toolCorner == ToolCorner::TOP_RIGHT) {
                y = floor(l->posY + l->height * (1 - factor));
            }
            top = data->toolCorner == ToolCorner::TOP_LEFT ||
                  data->toolCorner == ToolCorner::TOP_RIGHT ? cgreen : cred;
            bottom = data->toolCorner == ToolCorner::BOTTOM_LEFT ||
                     data->toolCorner == ToolCorner::BOTTOM_RIGHT ? cgreen : cred;
            left = data->toolCorner == ToolCorner::TOP_LEFT ||
                   data->toolCorner == ToolCorner::BOTTOM_LEFT ? cgreen : cred;
            right = data->toolCorner == ToolCorner::TOP_RIGHT ||
                    data->toolCorner == ToolCorner::BOTTOM_RIGHT ? cgreen : cred;
            drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, top, right, bottom, left);
            drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, top, right, bottom, left);
        } else if (data->currentTool == Tool::CROP) {
            x = l->posX + data->cropLeft;
            y = l->posY + data->cropTop;
            w = l->width - data->cropLeft - data->cropRight;
            h = l->height - data->cropTop - data->cropBottom;
            top = data->toolCorner == ToolCorner::TOP_LEFT ||
                  data->toolCorner == ToolCorner::TOP_RIGHT ? cgreen : cred;
            bottom = data->toolCorner == ToolCorner::BOTTOM_LEFT ||
                     data->toolCorner == ToolCorner::BOTTOM_RIGHT ? cgreen : cred;
            left = data->toolCorner == ToolCorner::TOP_LEFT ||
                   data->toolCorner == ToolCorner::BOTTOM_LEFT ? cgreen : cred;
            right = data->toolCorner == ToolCorner::TOP_RIGHT ||
                    data->toolCorner == ToolCorner::BOTTOM_RIGHT ? cgreen : cred;
            drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, top, right, bottom, left);
            drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, top, right, bottom, left);
        } else if (data->currentTool == Tool::ROTATE) {
            double angle = data->rotateDeg * PI / 180.0;
            int centerX = l->posX + l->width / 2, centerY = l->posY + l->height / 2;
            double cosAngle = cos(angle), sinAngle = sin(angle);
            int dx1 = l->posX - centerX, dy1 = l->posY - centerY;
            int dx2 = l->posX + l->width - centerX - 1, dy2 = l->posY + l->height - centerY - 1;
            int newX1 = centerX + dx1 * cosAngle - dy1 * sinAngle, newY1 =
                    centerY + dx1 * sinAngle + dy1 * cosAngle;
            int newX2 = centerX + dx2 * cosAngle - dy1 * sinAngle, newY2 =
                    centerY + dx2 * sinAngle + dy1 * cosAngle;
            int newX3 = centerX + dx2 * cosAngle - dy2 * sinAngle, newY3 =
                    centerY + dx2 * sinAngle + dy2 * cosAngle;
            int newX4 = centerX + dx1 * cosAngle - dy2 * sinAngle, newY4 =
                    centerY + dx1 * sinAngle + dy2 * cosAngle;
            drawOverlayBox(newX1 + 1, newY1 + 1, newX2 - 1, newY2 + 1, newX3 - 1, newY3 - 1, newX4, newY4 - 1, cgreen);
            drawOverlayBox(newX1, newY1, newX2, newY2, newX3, newY3, newX4 + 1, newY4, cgreen);
        } else if (data->currentTool == Tool::EXPORT_PNG || data->currentTool == Tool::EXPORT_JPG ||
                   data->currentTool == Tool::EXPORT_BMP || data->currentTool == Tool::NEW_EMPTY) {
            x = data->layerX, y = data->layerY, w = data->layerW, h = data->layerH;
            drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, cgreen);
            drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2,
                           cgreen);
        } else if (data->currentTool == Tool::COMBINE) {
            if (data->combineFirst < data->layers->countNum()) {
                Layer *l1 = data->layers->at(data->combineFirst);
                int x1 = l1->posX, y1 = l1->posY, w1 = l1->width, h1 = l1->height;
                drawOverlayBox(x1, y1, x1 + w1 - 1, y1, x1 + w1 - 1, y1 + h1 - 1, x1, y1 + h1 - 1, cgreen);
                drawOverlayBox(x1 + 1, y1 + 1, x1 + w1 - 2, y1 + 1, x1 + w1 - 2, y1 + h1 - 2, x1 + 1, y1 + h1 - 2, cgreen);
            }
            if (data->combineSecond < data->layers->countNum()) {
                Layer *l2 = data->layers->at(data->combineSecond);
                int x2 = l2->posX, y2 = l2->posY, w2 = l2->width, h2 = l2->height;
                drawOverlayBox(x2, y2, x2 + w2 - 1, y2, x2 + w2 - 1, y2 + h2 - 1, x2, y2 + h2 - 1, cgreen);
                drawOverlayBox(x2 + 1, y2 + 1, x2 + w2 - 2, y2 + 1, x2 + w2 - 2, y2 + h2 - 2, x2 + 1, y2 + h2 - 2, cgreen);
            }
        } else if (data->currentTool == Tool::DUPLICATE) {
            if (data->dupeIndex < data->layers->countNum()) {
                l = data->layers->at(data->dupeIndex);
                x = l->posX, y = l->posY, w = l->width, h = l->height;
                drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, cgreen);
                drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, cgreen);
            }
        } else if (data->currentTool == Tool::SHAPE) {
            x = data->shapeX, y = data->shapeY, w = data->shapeW, h = data->shapeH;
            if (data->currentShape == Shape::RECTANGLE) {
                drawOverlayBox(x, y, x + w - 1, y, x + w - 1, y + h - 1, x, y + h - 1, cgreen);
                drawOverlayBox(x + 1, y + 1, x + w - 2, y + 1, x + w - 2, y + h - 2, x + 1, y + h - 2, cgreen);
            } else if (data->currentShape == Shape::SQUARE) {
                int size = max(abs(w), abs(h));
                int newX = w < 0 ? x - size : x;
                int newY = h < 0 ? y - size : y;
                drawOverlayBox(newX, newY, newX + size - 1, newY, newX + size - 1, newY + size - 1, newX, newY + size - 1, cgreen);
                drawOverlayBox(newX + 1, newY + 1, newX + size - 2, newY + 1, newX + size - 2, newY + size - 2, newX + 1, newY + size - 2,
                               cgreen);
            } else if (data->currentShape == Shape::ELLIPSE) {
                int rx = abs(w) / 2;
                int ry = abs(h) / 2;
                int cx = x + (w > 0 ? rx : -rx);
                int cy = y + (h > 0 ? ry : -ry);

                for (double angle = 0; angle < 2 * PI; angle += 0.005) {
                    int px = cx + rx * cos(angle);
                    int py = cy + ry * sin(angle);
                    pixelDrawer_overlay->drawPixel(px, py, cgreen);
                    pixelDrawer_overlay->drawPixel(px + 1, py, cgreen);
                }
            } else if (data->currentShape == Shape::CIRCLE) {
                int size = max(abs(w), abs(h));
                int r = size / 2;
                int newX = w < 0 ? x - size : x;
                int newY = h < 0 ? y - size : y;
                int cx = newX + r;
                int cy = newY + r;

                for (double angle = 0; angle < 2 * PI; angle += 0.005) {
                    int px = cx + r * cos(angle);
                    int py = cy + r * sin(angle);
                    pixelDrawer_overlay->drawPixel(px, py, cgreen);
                    pixelDrawer_overlay->drawPixel(px + 1, py, cgreen);
                }
            }
        } else if (data->currentTool == Tool::REPLACE_COLOR) {
            x = data->replaceColorX, y = data->replaceColorY;
            int relX = x - l->posX, relY = y - l->posY;
            uint32_t oldColor = l->getPixel(relX, relY);
            drawFilledOverlayBox(x, y, x + 10, y, x + 10, y + 10, x, y + 10, oldColor);
            drawFilledOverlayBox(x + 1, y + 1, x + 9, y + 1, x + 9, y + 9, x + 1, y + 9, oldColor);
            drawOverlayBox(x, y, x + 10, y, x + 10, y + 10, x, y + 10, cgreen);
            drawOverlayBox(x + 1, y + 1, x + 9, y + 1, x + 9, y + 9, x + 1, y + 9, cgreen);
        }
    }

    // mouse Helper
    if (data->settings->showMouseHelper) {
        int xPos = data->workAreaX - 48, yPos = data->workAreaY - 48;
        auto xColor = data->currentTool == Tool::MOVE || data->currentTool == Tool::ROTATE || data->currentTool == Tool::SCALE ||
                      data->currentTool == Tool::CROP || data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER ||
                      data->currentTool == Tool::COLOR || data->currentTool == Tool::SHAPE || data->currentTool == Tool::REPLACE_COLOR ||
                      data->currentTool == Tool::EXPORT_PNG || data->currentTool == Tool::EXPORT_JPG ||
                      data->currentTool == Tool::EXPORT_BMP || data->currentTool == Tool::NEW_EMPTY ? cgreen : cgray;
        auto yColor = data->currentTool == Tool::MOVE || data->currentTool == Tool::SCALE ||
                      data->currentTool == Tool::CROP || data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER ||
                      data->currentTool == Tool::COLOR || data->currentTool == Tool::SHAPE || data->currentTool == Tool::REPLACE_COLOR ||
                      data->currentTool == Tool::EXPORT_PNG || data->currentTool == Tool::EXPORT_JPG ||
                      data->currentTool == Tool::EXPORT_BMP || data->currentTool == Tool::NEW_EMPTY ? cgreen : cgray;
        stringDrawer_overlay->drawMonoBitmap(xPos + 16, yPos + 0, 16, 16, yColor, cwhite, Bitmaps::arrow_up);
        stringDrawer_overlay->drawMonoBitmap(xPos + 0, yPos + 16, 16, 16, xColor, cwhite, Bitmaps::arrow_left);
        stringDrawer_overlay->drawMonoBitmap(xPos + 16, yPos + 16, 16, 16, cblack, cwhite, Bitmaps::mouse);
        stringDrawer_overlay->drawMonoBitmap(xPos + 32, yPos + 16, 16, 16, xColor, cwhite, Bitmaps::arrow_right);
        stringDrawer_overlay->drawMonoBitmap(xPos + 16, yPos + 32, 16, 16, yColor, cwhite, Bitmaps::arrow_down);
    }

    // debug String
    if (data->debugString != nullptr) {
        stringDrawer_overlay->drawString(Util::Graphic::Fonts::TERMINAL_8x16, 0, data->workAreaY - 16, data->debugString, cblack, cwhite);
    }
    data->flags->overlay = false;
}

void Renderer::renderLayers() {
    if (data->flags->layerOrder || !data->settings->optimizeRendering) {
        for (int i = 0; i < data->workAreaAll; i++) {
            buff_over_current[i] = 0x00000000;
            buff_under_current[i] = 0x00000000;
        }
        for (int i = 0; i < data->layers->currentNum(); i++) { // für buff_under_current
            auto layer = data->layers->at(i);
            if (layer->isVisible)
                blendBuffers(buff_under_current, layer->getPixelData(), data->workAreaX, data->workAreaY, layer->width, layer->height,
                             layer->posX, layer->posY);
        }
        for (int i = data->layers->currentNum() + 1; i < data->layers->countNum(); i++) { // für buff_over_current
            auto layer = data->layers->at(i);
            if (layer->isVisible)
                blendBuffers(buff_over_current, layer->getPixelData(), data->workAreaX, data->workAreaY, layer->width, layer->height,
                             layer->posX, layer->posY);
        }
    }
    for (int i = 0; i < data->workAreaAll; i++) {
        buff_layers[i] = 0x00000000;
    }
    blendBuffers(buff_layers, buff_under_current, data->workAreaAll);
    if (data->layers->currentNum() >= 0) {
        auto currentLayer = data->layers->current();
        if (currentLayer->isVisible)
            blendBuffers(buff_layers, currentLayer->getPixelData(), data->workAreaX, data->workAreaY,
                         currentLayer->width, currentLayer->height, currentLayer->posX, currentLayer->posY);
    }
    blendBuffers(buff_layers, buff_over_current, data->workAreaAll);
    data->flags->layers = false;
}

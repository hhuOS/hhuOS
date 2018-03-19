#include "GraphicsService.h"

LinearFrameBuffer *GraphicsService::getLinearFrameBuffer() {
    return lfb;
}

void GraphicsService::setLinearFrameBuffer(LinearFrameBuffer *lfb) {
    this->lfb = lfb;
}

TextDriver *GraphicsService::getTextDriver() {
    return text;
}

void GraphicsService::setTextDriver(TextDriver *text) {
    this->text = text;
}
#include <kernel/Kernel.h>
#include "GraphicsService.h"
#include "EventBus.h"

GraphicsService::GraphicsService() : textEventBuffer(16), lfbEventBuffer(16) {

}

LinearFrameBuffer *GraphicsService::getLinearFrameBuffer() {
    return lfb;
}

void GraphicsService::setLinearFrameBuffer(LinearFrameBuffer *lfb) {
    this->lfb = lfb;

    lfbEventBuffer.push(LfbDriverChangedEvent(lfb));

    Kernel::getService<EventBus>()->publish(lfbEventBuffer.pop());
}

TextDriver *GraphicsService::getTextDriver() {
    return text;
}

void GraphicsService::setTextDriver(TextDriver *text) {
    this->text = text;

    textEventBuffer.push(TextDriverChangedEvent(text));

    Kernel::getService<EventBus>()->publish(textEventBuffer.pop());
}

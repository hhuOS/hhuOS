/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <kernel/Kernel.h>
#include "GraphicsService.h"
#include "EventBus.h"

GraphicsService::GraphicsService() : textEventBuffer(16), lfbEventBuffer(16) {

}

LinearFrameBuffer *GraphicsService::getLinearFrameBuffer() {
    return lfb;
}

void GraphicsService::setLinearFrameBuffer(LinearFrameBuffer *lfb) {
    if(this->lfb != nullptr) {
        lfb->init(this->lfb->getResX(), this->lfb->getResY(), this->lfb->getDepth());
    }

    this->lfb = lfb;

    lfbEventBuffer.push(LfbDriverChangedEvent(lfb));

    Kernel::getService<EventBus>()->publish(lfbEventBuffer.pop());
}

TextDriver *GraphicsService::getTextDriver() {
    return text;
}

void GraphicsService::setTextDriver(TextDriver *text) {
    if(this->text != nullptr) {
        text->init(this->text->getColumnCount(), this->text->getRowCount(), this->lfb->getDepth());
    }

    this->text = text;

    textEventBuffer.push(TextDriverChangedEvent(text));

    Kernel::getService<EventBus>()->publish(textEventBuffer.pop());
}

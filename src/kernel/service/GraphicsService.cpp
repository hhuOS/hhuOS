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

#include "kernel/core/System.h"
#include "GraphicsService.h"
#include "EventBus.h"

namespace Kernel {

Logger &GraphicsService::log = Logger::get("GRAPHICS");

void GraphicsService::registerLinearFrameBuffer(LinearFrameBuffer *lfb) {
    if (lfb != nullptr) {
        lfbMap.put(lfb->getName().toLowerCase(), lfb);

        log.info("Registered '%s' as LinearFrameBuffer", (const char *) lfb->getName());
    }
}

void GraphicsService::registerTextDriver(TextDriver *text) {
    if (text != nullptr) {
        textMap.put(text->getName().toLowerCase(), text);

        log.info("Registered '%s' as TextDriver", (const char *) text->getName());
    }
}

Util::Array<String> GraphicsService::getAvailableLinearFrameBuffers() {
    return lfbMap.keySet();
}

Util::Array<String> GraphicsService::getAvailableTextDrivers() {
    return textMap.keySet();
}

bool GraphicsService::setLinearFrameBuffer(const String &name) {
    String driverName = name;
    driverName = driverName.toLowerCase();

    if (!lfbMap.containsKey(driverName)) {
        log.warn("No implementation of LinearFrameBuffer registered under the name '%s'", (const char *) name);

        return false;
    } else {
        currentLfb = lfbMap.get(driverName);

        log.info("LinearFrameBuffer set to '%s'", (const char *) currentLfb->getName());

        return true;
    }
}

bool GraphicsService::setTextDriver(const String &name) {
    String driverName = name;
    driverName = driverName.toLowerCase();

    if (!textMap.containsKey(driverName)) {
        log.warn("No implementation of TextDriver registered under the name '%s'", (const char *) name);

        return false;
    } else {
        currentTextDriver = textMap.get(driverName);

        log.info("TextDriver set to '%s'", (const char *) currentTextDriver->getName());

        return true;
    }
}

LinearFrameBuffer *GraphicsService::getLinearFrameBuffer() {
    return currentLfb;
}

TextDriver *GraphicsService::getTextDriver() {
    return currentTextDriver;
}

}
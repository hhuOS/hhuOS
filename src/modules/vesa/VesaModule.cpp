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

#include <kernel/services/GraphicsService.h>
#include "VesaModule.h"

MODULE_PROVIDER {

    return new VesaModule();
};

int32_t VesaModule::initialize() {

    auto *graphicsService = Kernel::getService<GraphicsService>();
    
    auto *lfb = new VesaGraphics();
    auto *text = new VesaText();
    
    if(lfb->isAvailable()) {
        graphicsService->registerLinearFrameBuffer(lfb);
        graphicsService->registerTextDriver(text);
    }

    return 0;
}

int32_t VesaModule::finalize() {

    delete lfb;
    delete text;

    return 0;
}

String VesaModule::getName() {

    return "VesaDriver";
}

Util::Array<String> VesaModule::getDependencies() {

    return Util::Array<String>(0);
}

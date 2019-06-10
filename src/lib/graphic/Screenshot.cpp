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

#include "lib/file/bmp/Bmp.h"
#include "kernel/service/GraphicsService.h"
#include "Screenshot.h"

Screenshot::~Screenshot() {
    if(pixelBuf != nullptr) {
        delete pixelBuf;
    }
}

void Screenshot::take() {
    LinearFrameBuffer *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
    bool isDoubleBuffered = lfb->isDoubleBuffered();

    width = lfb->getResX();
    height = lfb->getResY();
    depth = 32;

    if(pixelBuf != nullptr) {
        delete pixelBuf;
    }

    pixelBuf = new Color[width * height];

    if(isDoubleBuffered) {
        lfb->disableDoubleBuffering();
    }

    for(uint16_t i = 0; i < height; i++) {
        for(uint16_t j = 0; j < width; j++) {
            lfb->readPixel(j, i, pixelBuf[i * width + j]);
        }
    }

    if(isDoubleBuffered) {
        lfb->enableDoubleBuffering();
    }
}

void Screenshot::saveToBmp(const String &path) {
    if(pixelBuf == nullptr) {
        return;
    }

    Bmp bmp(pixelBuf, width, height);
    bmp.saveToFile(path);
}

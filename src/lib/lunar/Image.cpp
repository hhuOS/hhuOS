/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "Image.h"

#include <util/graphic/BitmapFile.h>

namespace Lunar {

Image::Image(const Util::String &path, const size_t width, const size_t height) :
    Widget(false, false)
{
    const auto *bitmap = Util::Graphic::BitmapFile::open(path);
    if (bitmap == nullptr) {
        originalImage = new Util::Graphic::Image(0, 0, nullptr);
        delete bitmap;
    } else {
        originalImage = bitmap->scale(width, height);
        delete bitmap;
    }
}

Image::~Image() {
    delete originalImage;
    delete scaledDownImage;
}

void Image::setSize(const size_t width, const size_t height) {
    if (width < originalImage->getWidth() || height < originalImage->getHeight()) {
        scaledDownImage = originalImage->scale(width, height);
        Widget::setSize(width, height);
    } else {
        Widget::setSize(originalImage->getWidth(), originalImage->getHeight());
    }
}

void Image::draw(const Util::Graphic::LinearFrameBuffer &lfb) {
    if (getWidth() < originalImage->getWidth() || getHeight() < originalImage->getHeight()) {
        lfb.drawImage(*scaledDownImage, getPosX(), getPosY());
    } else {
        lfb.drawImage(*originalImage, getPosX(), getPosY());
    }

    Widget::draw(lfb);
}

}
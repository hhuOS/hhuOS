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

#ifndef HHUOS_LIB_LUNAR_IMAGE_H
#define HHUOS_LIB_LUNAR_IMAGE_H

#include <stddef.h>

#include "Widget.h"

#include <util/base/String.h>
#include <util/graphic/Image.h>

namespace Lunar {

/// A widget that displays an image.
/// The image can be scaled to a specific width and height.
class Image : public Widget {

public:
    /// Create a new image widget from the given image file.
    /// The image will be scaled to the given width and height.
    Image(const Util::String &path, size_t width, size_t height);

    /// Create a new image widget from an already loaded image.
    /// The widget takes ownership of the image instance and deletes it in its destructor.
    explicit Image(const Util::Graphic::Image *image);

    /// Destroy the image widget, freeing up memory used by the image buffer.
    ~Image() override;

    /// Load the image to display from the given image file.
    /// The image will be scaled to the given width and height.
    void setImage(const Util::String &path, size_t width, size_t height);

    /// Set the image to display to the given image.
    /// The widget takes ownership of the image instance and deletes it in its destructor.
    void setImage(const Util::Graphic::Image *image);

    /// Get the preferred width of the image.
    /// The preferred width is the image width given in the constructor.
    size_t getPreferredWidth() const override {
        return originalImage->getWidth();
    }

    /// Get the preferred height of the image.
    /// The preferred width is the height width given in the constructor.
    size_t getPreferredHeight() const override {
        return originalImage->getHeight();
    }

    /// Set the size of the image.
    /// If the new size is larger than the original, the image does not scale.
    /// If the new size is smaller that the original, the image is scaled down.
    void setSize(size_t width, size_t height) override;

    /// Draw the image on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    const Util::Graphic::Image *originalImage = nullptr;
    const Util::Graphic::Image *scaledDownImage = nullptr;
};

}

#endif

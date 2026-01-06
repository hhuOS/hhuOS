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

#ifndef HHUOS_WIDGET_APPLICATION_H
#define HHUOS_WIDGET_APPLICATION_H

#include <stddef.h>

#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "lunar/Container.h"
#include "util/io/key/KeyDecoder.h"
#include "util/io/key/layout/DeLayout.h"
#include "util/io/stream/FileInputStream.h"

class WidgetApplication : public Lunar::Container {

public:

    WidgetApplication(Util::Graphic::LinearFrameBuffer &lfb, size_t width, size_t height);

    void pack();

    void setSize(size_t width, size_t height) override;

protected:

    void update();

private:

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Graphic::BufferedLinearFrameBuffer bufferedLfb;

    Util::Io::KeyDecoder keyDecoder = Util::Io::KeyDecoder(Util::Io::DeLayout());
    Util::Io::FileInputStream mouseInputStream;

    Widget *lastHoveredWidget = nullptr;
    Widget *lastPressedWidget = nullptr;

    int32_t mouseX = static_cast<int32_t>(getPosX() + getWidth() / 2);
    int32_t mouseY = static_cast<int32_t>(getPosY() + getHeight() / 2);
    bool mouseButtonLeft = false;

    uint8_t mouseInput[4]{};
    size_t mouseInputIndex = 0;
};

#endif
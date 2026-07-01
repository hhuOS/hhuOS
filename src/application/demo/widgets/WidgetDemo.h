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

#ifndef HHUOS_APPLICATION_DEMO_WIDGETDEMO_H
#define HHUOS_APPLICATION_DEMO_WIDGETDEMO_H

#include "WidgetApplication.h"

#include <util/graphic/LinearFrameBuffer.h>
#include <lunar/Label.h>

/// A demo that showcases the Lunar widget library.
/// It renders multiple different widgets (e.g., buttons, labels, etc.) that react to mouse and keyboard input.
class WidgetDemo : public WidgetApplication {

public:
    /// Create a new widget demo instance that renders to the given framebuffer.
    explicit WidgetDemo(Util::Graphic::LinearFrameBuffer &lfb);

    /// Run the demo by creating all widgets and entering the main loop.
    void run();

private:

    size_t lastFps = 0;
};


#endif
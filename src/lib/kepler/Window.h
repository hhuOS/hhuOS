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

#ifndef HHUOS_LIB_KEPLER_WINDOW_H
#define HHUOS_LIB_KEPLER_WINDOW_H

#include <stdint.h>

#include "MouseRunnable.h"
#include "util/async/SharedMemory.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "kepler/WindowManagerPipe.h"

namespace Kepler {

class Window {

public:

    Window(uint16_t width, uint16_t height, const Util::String &title, WindowManagerPipe &pipe);

    Window(const Window &other) = delete;

    Window& operator=(const Window &other) = delete;

    ~Window();

    [[nodiscard]] Util::Graphic::LinearFrameBuffer& getFrameBuffer() const;

    bool flush() const;

    void registerMouseListener(MouseListener &listener) const {
        mouseRunnable->registerListener(listener);
    }

private:

    size_t id = 0;

    WindowManagerPipe &pipe;
    MouseRunnable *mouseRunnable = nullptr;

    Util::Async::SharedMemory *sharedMemory = nullptr;
    Util::Graphic::LinearFrameBuffer *lfb = nullptr;
};

}

#endif
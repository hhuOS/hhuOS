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

#ifndef HHUOS_MOUSERUNNABLE_H
#define HHUOS_MOUSERUNNABLE_H

#include "MouseListener.h"
#include "util/async/Runnable.h"
#include "util/base/String.h"
#include "util/io/stream/FileInputStream.h"

namespace Kepler {

class MouseRunnable final : public Util::Async::Runnable {

public:

    explicit MouseRunnable(const Util::String &pipePath) : mouseInputStream(pipePath) {}

    void run() override;

    void stop() {
        isRunning = false;
    }

    void registerListener(MouseListener &listener) {
        if (MouseRunnable::listener != nullptr) {
            Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "MouseRunnable: Listener already registered");
        }

        MouseRunnable::listener = &listener;
    }

private:

    Util::Io::FileInputStream mouseInputStream;
    MouseListener *listener = nullptr;
    bool isRunning = true;
};

}

#endif
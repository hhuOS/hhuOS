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

#ifndef HHUOS_WINDOWSTACK_H
#define HHUOS_WINDOWSTACK_H

#include "ClientWindow.h"
#include "util/collection/ArrayList.h"

class WindowStack : public Util::Iterable<ClientWindow*> {

public:

    WindowStack() = default;

    ~WindowStack() override = default;

    void push(ClientWindow *window);

    ClientWindow* getFocussedWindow() const {
        return windows.size() > 0 ? windows.get(windows.size() - 1) : nullptr;
    }

    void setFocus(ClientWindow *window);

    bool isFocussed(const ClientWindow *window) const {
        const auto *focussedWindow = getFocussedWindow();
        return window == focussedWindow;
    }

    ClientWindow* getWindowAt(uint16_t x, uint16_t y) const;

    ClientWindow* getWindowById(size_t id) const;

    Util::Iterator<ClientWindow*> begin() const override {
        return windows.begin();
    }

    Util::Iterator<ClientWindow*> end() const override {
        return windows.end();
    }

    Util::IteratorElement<ClientWindow*> next(const Util::IteratorElement<ClientWindow*> &element) const override {
        return windows.next(element);
    }

private:

    Util::ArrayList<ClientWindow*> windows;
};

#endif
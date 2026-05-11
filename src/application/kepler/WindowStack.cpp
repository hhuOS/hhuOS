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

#include "WindowStack.h"

#include "ClientWindow.h"

void WindowStack::push(ClientWindow *window) {
    if (windows.size() > 0) {
        windows.get(windows.size() - 1)->setDirty();
    }

    windows.add(window);
}

void WindowStack::remove(ClientWindow *window) {
    windows.remove(window);
    delete window;
}

void WindowStack::setFocus(ClientWindow *window) {
    auto *oldFocus = getFocusedWindow();
    if (window != oldFocus) {
        window->setDirty();
        oldFocus->setDirty();
        windows.remove(window);
        windows.add(window);
    }
}

ClientWindow* WindowStack::getWindowAt(const uint16_t x, const uint16_t y) const {
    for (size_t i = windows.size() - 1; i < windows.size(); i--) {
        auto *window = windows.get(i);
        const auto mouseEvent = window->containsPoint(x, y);
        if (mouseEvent.area != NONE) {
            return window;
        }
    }

    return nullptr;
}

ClientWindow* WindowStack::getWindowById(const size_t id) const {
    for (auto *window : windows) {
        if (window->getId() == id) {
            return window;
        }
    }

    return nullptr;
}

void WindowStack::markWindowsOnTopDirty(ClientWindow *window) const {
    // Run through the window stack (back to front), skipping all windows behind the requested window.
    // All windows on top of the request window are marked dirty if they overlap with it
    // or any other window that needs to be redrawn
    bool windowFound = false;
    Util::ArrayList<ClientWindow*> overlappingWindows; // List of windows that need to be redrawn
    for (auto *currentWindow : windows) {
        if (windowFound) {
            for (const auto *dirtyWindow : overlappingWindows) {
                if (currentWindow->overlapsWith(*dirtyWindow)) {
                    // Window is drawn on top of another dirty window -> Mark dirty
                    overlappingWindows.add(currentWindow);
                    currentWindow->setDirty();
                }
            }
        } else if (currentWindow == window) {
            // Requested window found
            overlappingWindows.add(window);
            windowFound = true;
        }
    }
}

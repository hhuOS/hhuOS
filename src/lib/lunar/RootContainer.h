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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#ifndef HHUOS_LIB_LUNAR_ROOTCONTAINER_H
#define HHUOS_LIB_LUNAR_ROOTCONTAINER_H

#include "Container.h"

#include <util/io/key/MouseDecoder.h>

namespace Lunar {

/// A container that can handle mouse and keyboard events.
/// It is useful as the root of a layout tree, as it forwards any events to the appropriate child widget.
class RootContainer : public Container {

public:
    /// Create a new root container instance with the given width and height.
    RootContainer(const size_t width, const size_t height) : Container(width, height) {}

    /// Destroy the root container instance.
    ~RootContainer() override = default;

    /// Handle a mouse hover event.
    /// The event is forwarded to the widget at the given position inside the container.
    void onMouseHover(uint16_t x, uint16_t y);

    /// Handle a mouse click event.
    /// The event is forwarded to the widget at the given position inside the container.
    void onMouseClick(uint16_t x, uint16_t y, Util::Io::MouseDecoder::Button button, bool pressed);

    /// Handle a keyboard event.
    /// The event is forwarded to focussed widget (i.e., the widget that has lastly been clicked on).
    void onKeyEvent(const Util::Io::KeyEvent &keyEvent) const;

private:

    Widget *lastHoveredChild = nullptr;
    Widget *lastPressedChild = nullptr;
};

}

#endif

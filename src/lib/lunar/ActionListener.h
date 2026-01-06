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

#ifndef HHUOS_LIB_LUNAR_ACTIONLISTENER_H
#define HHUOS_LIB_LUNAR_ACTIONLISTENER_H

#include "util/io/key/Key.h"

namespace Lunar {

/// An action listener notifies a widget about certain events, such as mouse clicks or key presses.
/// Action listeners can be registered with a widget via `Widget::addActionListener()`.
/// When an event occurs on the widget, the corresponding method is called on all registered action listeners.
/// The base class provides empty implementations for all methods,
/// so subclasses can override only the methods they are interested in.
class ActionListener {

public:
    /// Create a new action listener instance.
    ActionListener() = default;

    /// Destroy the action listener instance.
    virtual ~ActionListener() = default;

    /// Notify the listener that the mouse has entered the widget's area.
    virtual void onMouseEntered() {}

    /// Notify the listener that the mouse has exited the widget's area.
    virtual void onMouseExited() {}

    /// Notify the listener that a mouse button has been pressed while the mouse is over the widget.
    virtual void onMousePressed() {}

    /// Notify the listener that a mouse button has been released while the mouse is over the widget.
    virtual void onMouseReleased() {}

    /// Notify the listener that the widget has been clicked with the mouse.
    /// A mouse click is defined as a mouse press followed by a mouse release while the mouse is over the widget.
    virtual void onMouseClicked() {}

    /// Notify the listener that a key has been pressed while the widget has keyboard focus.
    virtual void onKeyPressed(const Util::Io::Key&) {}

    /// Notify the listener that a key has been released while the widget has keyboard focus.
    virtual void onKeyReleased(const Util::Io::Key&) {}

    /// Notify the listener that a key has been typed while the widget has keyboard focus.
    /// A key is considered typed when it is pressed and then released.
    virtual void onKeyTyped(const Util::Io::Key&) {}
};

}

#endif
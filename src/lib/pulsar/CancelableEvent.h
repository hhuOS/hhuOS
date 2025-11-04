/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_UTIL_GAME_CANCELABLEEVENT_H
#define HHUOS_LIB_UTIL_GAME_CANCELABLEEVENT_H

namespace Pulsar {

/// A subclass of `Event` that can be canceled.
/// For example, `TranslationEvent` is a cancelable event. One example where a `TranslationEvent` occurs,
/// is if an entity has a dynamic collider and collides with another entity. In this case, a `TranslationEvent` is
/// created to move the entity away from the collision. This translation can then be canceled
/// by the collision event handler, preventing the entity from being moved.
class CancelableEvent {

public:
    /// Create a new cancelable event instance.
    CancelableEvent() = default;

    /// Cancel the event. This can be called by event handlers to prevent the action from being performed
    /// (e.g. prevent an entity from being moved by a `TranslationEvent`).
    void cancel();

    /// Check if the event has been canceled.
    [[nodiscard]] bool isCanceled() const;

private:

    bool canceled = false;
};

}

#endif

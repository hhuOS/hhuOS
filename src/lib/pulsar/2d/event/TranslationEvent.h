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

#ifndef HHUOS_LIB_PULSAR_2D_TRANSLATIONEVENT_H
#define HHUOS_LIB_PULSAR_2D_TRANSLATIONEVENT_H

#include "util/math/Vector2.h"
#include "pulsar/CancelableEvent.h"

namespace Pulsar::D2 {

/// A translation event for 2D entities.
/// This event is triggered when an entity is about to be moved to a new position by a component attached to it,
/// or via the `Entity::translate()` method.
/// If an entity is moved directly via `Entity::setPosition()`, no translation event is created.
/// The event contains the target position the entity is being moved to.
/// Translation events are cancelable and if translation event is canceled,
/// the entity will not be moved to the target position.
class TranslationEvent final : public CancelableEvent {

public:
    /// Create a new translation event instance.
    /// Translation events are created automatically by components that move entities.
    /// Thus, this constructor is intended for internal use only.
    explicit TranslationEvent(const Util::Math::Vector2<float> &targetPosition);

    /// Get the target position the entity is being moved to.
    [[nodiscard]] const Util::Math::Vector2<float>& getTargetPosition() const;

private:

    Util::Math::Vector2<float> targetPosition;
};

}

#endif

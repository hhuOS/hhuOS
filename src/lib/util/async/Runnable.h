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
 */

#ifndef HHUOS_LIB_UTIL_ASYNC_RUNNABLE_H
#define HHUOS_LIB_UTIL_ASYNC_RUNNABLE_H

namespace Util::Async {

/// Base class for a Runnable, which can be run in a thread.
class Runnable {

public:
    /// The Runnable base class has no state, so the default constructor is sufficient.
    Runnable() = default;

    /// The Runnable base class has no state, so the default destructor is sufficient.
    virtual ~Runnable() = default;

    /// Run the Runnable.
    /// This is called by a thread when it starts.
    virtual void run() = 0;
};

}

#endif
/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_THREADUTIL_H
#define HHUOS_THREADUTIL_H

namespace Util::Async {

class ThreadUtil {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    ThreadUtil() = delete;

    /**
     * Copy constructor.
     */
    ThreadUtil(const ThreadUtil &other) = delete;

    /**
     * Assignment operator.
     */
    ThreadUtil &operator=(const ThreadUtil &other) = delete;

    /**
     * Destructor.
     */
    ~ThreadUtil() = delete;

    static void yield();

    static void exitProcess(int32_t exitCode);
};

}

#endif
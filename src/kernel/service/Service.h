/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __KernelService_include__
#define __KernelService_include__

namespace Kernel {

/**
 * KernelService - Base class for all Kernel services.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Service {

public:
    /**
     * Default Constructor.
     */
    Service() = default;

    /**
     * Copy Constructor.
     */
    Service(const Service &copy) = delete;

    /**
     * Assignment operator.
     */
    Service& operator=(const Service &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Service() = default;

};

}

#endif

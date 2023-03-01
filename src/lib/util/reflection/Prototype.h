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

#ifndef HHUOS_PROTOTYPE_H
#define HHUOS_PROTOTYPE_H

#define PROTOTYPE_IMPLEMENT_CLONE(TYPE) Prototype *clone() const override { return new TYPE(); }
#define PROTOTYPE_IMPLEMENT_GET_CLASS_NAME(CLASS_NAME) Util::String getClassName() const override { return CLASS_NAME; }

#include "lib/util/base/String.h"

namespace Util::Reflection {

/**
 * Implementation of the prototype pattern, based on
 * http://www.cs.sjsu.edu/faculty/pearce/modules/lectures/oop/types/reflection/prototype.htm
 */
class Prototype {

public:
    /**
     * Constructor.
     */
    Prototype() = default;

    /**
     * Destructor.
     */
    virtual ~Prototype() = default;

    /**
     * Get the name, under which the prototype will be registered and usable for the user.
     */
    [[nodiscard]] virtual String getClassName() const = 0;

private:
    /**
     * Create a copy of this instance.
     *
     * @return A pointer to the copy
     */
    [[nodiscard]] virtual Prototype *clone() const;

    friend class InstanceFactory;
};

}

#endif

/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_INSTANCEFACTORY_H
#define HHUOS_INSTANCEFACTORY_H

#include "lib/util/memory/String.h"

namespace Util {
namespace Data {
template <typename K, typename V> class HashMap;
}  // namespace Data
namespace Reflection {
class Prototype;
}  // namespace Reflection
}  // namespace Util

#define INSTANCE_FACTORY_CREATE_INSTANCE(BASE_TYPE, TYPE) (BASE_TYPE*) Util::Reflection::InstanceFactory::createInstance(TYPE)

namespace Util::Reflection {

/**
 * Implementation of the prototype pattern, based on
 * http://www.cs.sjsu.edu/faculty/pearce/modules/lectures/oop/types/reflection/prototype.htm
 */
class InstanceFactory {

public:
    /**
     * Create a new instance of a given prototype.
     * Returns nullptr, if the type is unknown.
     *
     * @param type The type
     *
     * @return A pointer to newly created instance
     */
    static Prototype* createInstance(const Memory::String &type);

    /**
     * Remove a prototype.
     *
     * @param type The type
     */
    static void deregisterPrototype(const Memory::String &type);

    /**
     * Add a prototype.
     * Instances of this type can then be created by calling 'Prototype::createInstance(type)'.
     *
     * @param type The type
     * @param prototype Instance, that will be used as a prototype for further instances
     */
    static void registerPrototype(Prototype *prototype);

private:

    /**
     * Contains prototypes for all available implementations.
     */
    static Data::HashMap<Memory::String, Prototype*> prototypeTable;
};

}

#endif

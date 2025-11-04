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

#ifndef HHUOS_LIB_UTIL_REFLECTION_INSTANCEFACTORY_H
#define HHUOS_LIB_UTIL_REFLECTION_INSTANCEFACTORY_H

#include "util/base/String.h"
#include "util/collection/HashMap.h"
#include "util/reflection/Prototype.h"

namespace Util::Reflection {

/// Static class that holds prototypes for all available `Prototype` implementations
/// and allows instantiation of these classes at runtime.
/// See the `Prototype` class for a discrete example on how to use prototypes.
class InstanceFactory {

public:
    /// Create a new instance of a prototype based on its class name.
    /// If the prototype is not registered, a panic is fired.
    /// Use `isPrototypeRegistered()` to check if a prototype is registered before calling this method.
    template <typename T>
    static T* createInstance(const String &className) {
        return static_cast<T*>(prototypeTable.get(className)->clone());
    }

    /// Check if a prototype is registered.
    static bool isPrototypeRegistered(const String &className);

    /// Remove a prototype from the factory.
    /// Instances of this type can no longer be created after this call.
    static void deregisterPrototype(const String &className);

    /// Register a prototype.
    /// Instances of this type can then be created by calling `createInstance()`.
    static void registerPrototype(Prototype *prototype);

private:

    static HashMap<String, Prototype*> prototypeTable;
};

}

#endif

/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#define PROTOTYPE_IMPLEMENT_CLONE(TYPE) Prototype *clone() const override { return new TYPE(*this); }

#include <lib/util/HashMap.h>
#include "String.h"

class Prototype {

private:

    /**
     * Contains prototypes for all available implementations.
     */
    static Util::HashMap<String, Prototype*> prototypeTable;

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
     * Create a copy of this instance.
     *
     * @return A pointer to the copy
     */
    virtual Prototype *clone() const = 0;

    /**
     * Get the name, under which the prototype will be registered and usable for the user.
     */
    virtual String getName() = 0;

    /**
     * Create a new instance of a given prototype.
     * Throws an exception, if the type is unknown.
     *
     * @param type The type
     *
     * @return A pointer to newly created instance
     */
    static Prototype *createInstance(String type);

    /**
     * Add a prototype.
     * Instances of this type can then be created by calling 'Prototype::createInstance(type)'.
     *
     * @param type The type
     * @param prototype Instance, that will be used as a prototype for further instances
     */
    static void registerPrototype(Prototype *prototype);

    /**
     * Remove a prototype.
     *
     * @param type The type
     */
    static void deregisterPrototype(String type);

};

#endif

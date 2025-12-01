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

#ifndef HHUOS_LIB_UTIL_REFLECTION_PROTOTYPE_H
#define HHUOS_LIB_UTIL_REFLECTION_PROTOTYPE_H

#define PROTOTYPE_IMPLEMENT_CLONE(TYPE) Prototype *clone() const override { return new TYPE(); }
#define PROTOTYPE_IMPLEMENT_GET_CLASS_NAME(CLASS_NAME) Util::String getClassName() const override { return CLASS_NAME; }

#include "util/base/String.h"

namespace Util::Reflection {

/// Implementation of the prototype pattern, based on
/// http://www.cs.sjsu.edu/faculty/pearce/modules/lectures/oop/types/reflection/prototype.htm.
///
/// It can be used to create new instances of a class without knowing the exact type at compile time.
/// A class that implements `Prototype` must implement the `getClassName()` method to return a unique name
/// and the `clone()` method to create a default instance of the class. Since `clone()` does not take any parameters,
/// the implementing class must have a default constructor.
/// The macros `PROTOTYPE_IMPLEMENT_CLONE` and `PROTOTYPE_IMPLEMENT_GET_CLASS_NAME`
/// can be used to implement these methods easily.
///
/// ## Example
/// ```c++
/// // Base class for fruits with a virtual method to get the price.
/// // It uses the prototype pattern to allow dynamic instantiation of fruit types based on user input.
/// class Fruit : public Util::Reflection::Prototype {
/// public:
///     Fruit() = default;
///     virtual float getPrice() const = 0;
/// };
///
/// // Concrete fruit classes that implement the getPrice method.
/// class Apple : public Fruit {
/// public:
///     Apple() = default;
///     float getPrice() const override { return 0.8f; }
///     PROTOTYPE_IMPLEMENT_CLONE(Apple)
///     PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Apple")
/// };
///
/// class Pear : public Fruit {
/// public:
///     Pear() = default;
///     float getPrice() const override { return 1.0f; }
///     PROTOTYPE_IMPLEMENT_CLONE(Pear)
///     PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Pear")
/// };
///
/// class Banana : public Fruit {
/// public:
///     Banana() = default;
///     float getPrice() const override { return 1.2f; }
///     PROTOTYPE_IMPLEMENT_CLONE(Banana)
///     PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Banana")
/// };
///
/// int main(int argc, char *argv[]) {
///     // Register the fruit prototypes with the InstanceFactory so they can be created dynamically.
///     Util::Reflection::InstanceFactory::registerPrototype(new Apple());
///     Util::Reflection::InstanceFactory::registerPrototype(new Pear());
///     Util::Reflection::InstanceFactory::registerPrototype(new Banana());
///
///     // Read fruit names from standard input and create instances dynamically.
///     auto input = Util::System::in.readLine();
///     while (!input.endOfFile) {
///         const auto fruitName = input.content;
///
///         // Check if the input is a valid fruit type.
///         if (!Util::Reflection::InstanceFactory::isPrototypeRegistered(fruitName)) {
///             Util::System::out << "Unknown fruit type: " << fruitName
///                 << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///             continue;
///         }
///
///         // Create an instance of the fruit based on the input string.
///         const auto *fruit = Util::Reflection::InstanceFactory::createInstance<Fruit>(fruitName);
///         Util::System::out << "Price of " << fruitName << ": " << fruit->getPrice()
///             << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///         delete fruit;
///
///         // Read the next line from standard input.
///         input = Util::System::in.readLine();
///     }
///
///     return 0;
/// }
/// ```
class Prototype {

public:
    /// The base prototype class has no state, so the default constructor is sufficient.
    Prototype() = default;

    /// The base prototype class has no state, so the default destructor is sufficient.
    virtual ~Prototype() = default;

    /// Get the class name of the prototype.
    /// Can easily be implemented using the `PROTOTYPE_IMPLEMENT_GET_CLASS_NAME` macro.
    virtual String getClassName() const = 0;

private:
    /// Create a new instance of the prototype.
    /// Can easily be implemented using the `PROTOTYPE_IMPLEMENT_CLONE` macro.
    /// This method is private to ensure that only the `InstanceFactory` can call it.
    virtual Prototype* clone() const = 0;

    friend class InstanceFactory;
};

}

#endif

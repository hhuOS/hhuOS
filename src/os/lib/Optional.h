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

#ifndef __Optional_include__
#define __Optional_include__

/**
 * @author Filip Krakowski
 */
template <typename T>
class Optional {

public:

    Optional() = default;

    explicit Optional(const T &element);

    bool isNull();

    T value();

private:

    T element;

    bool isInitialized = false;

};

template <class T>
Optional<T>::Optional(const T &element) {
    this->element = element;
    this->isInitialized = true;
}

template <class T>
bool Optional<T>::isNull() {
    return !isInitialized;
}

template <class T>
T Optional<T>::value() {
    return element;
}


#endif

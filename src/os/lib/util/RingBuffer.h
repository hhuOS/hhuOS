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

#ifndef __RingBuffer_include__
#define __RingBuffer_include__

#include "Collection.h"

namespace Util {

    /**
     * A simple ring buffer implementation.
     *
     * @author Filip Krakowski
     */
    template<typename T>
    class RingBuffer {

    public:

        explicit RingBuffer(uint32_t capacity);

        RingBuffer(const RingBuffer<T> &other) = delete;

        RingBuffer<T> &operator=(const RingBuffer<T> &other) = delete;

        ~RingBuffer() = default;

        void push(const T &element);

        T& pop();

        bool isEmpty() const;

    private:

        Array<T> buffer;

        uint32_t capacity;

        uint32_t readIndex;

        uint32_t writeIndex;

        uint32_t size;

    };

    template <class T>
    RingBuffer<T>::RingBuffer(uint32_t capacity) : buffer(capacity), capacity(capacity), readIndex(0), writeIndex(0), size(0)  {

        if (capacity == 0) {
            Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT);
        }
    }

    template <class T>
    void RingBuffer<T>::push(const T &element) {

        buffer[writeIndex] = element;

        writeIndex++;

        if (writeIndex >= capacity) {

            writeIndex = 0;
        }

        size++;
    }

    template <class T>
    T &RingBuffer<T>::pop() {

        T &tmp = buffer[readIndex];

        readIndex++;

        if (readIndex >= capacity) {

            readIndex = 0;
        }

        size--;

        return tmp;
    }

    template <class T>
    bool RingBuffer<T>::isEmpty() const {

        return size == 0;
    }

}




#endif

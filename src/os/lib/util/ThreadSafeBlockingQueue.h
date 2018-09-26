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

#ifndef __ThreadSafeBlockingQueue_include__
#define __ThreadSafeBlockingQueue_include__

#include <lib/lock/Mutex.h>
#include "Queue.h"
#include "ArrayList.h"

namespace Util {

/**
 * An implementation of the Queue interface.
 *
 * @author Filip Krakowski, Fabian Ruhland
 */
template <typename T>
class ThreadSafeBlockingQueue : public Queue<T> {

public:

    ThreadSafeBlockingQueue();

    explicit ThreadSafeBlockingQueue(uint32_t capacity);

    ~ThreadSafeBlockingQueue();

    ThreadSafeBlockingQueue(const ThreadSafeBlockingQueue<T> &other) = delete;

    ThreadSafeBlockingQueue<T> &operator=(const ThreadSafeBlockingQueue<T> &other) = delete;

    void push(const T &element) override;

    T pop() override;

    bool add(const T &element) override;

    bool addAll(const Collection<T> &other) override;

    bool remove(const T &element) override;

    bool removeAll(const Collection<T> &other) override;

    bool contains(const T &element) const override;

    bool containsAll(const Collection<T> &other) const override;

    bool isEmpty() const override;

    void clear() override;

    Iterator<T> begin() const override;

    Iterator<T> end() const override;

    uint32_t size() const override;

    Array<T> toArray() const override;

private:

    ArrayList<T> elements;
    
    Mutex *mutex = nullptr;

    static const uint32_t DEFAULT_CAPACITY = 16;

};

template<class T>
ThreadSafeBlockingQueue<T>::ThreadSafeBlockingQueue() : elements(DEFAULT_CAPACITY), mutex(new Mutex()) {
    
}

template<class T>
ThreadSafeBlockingQueue<T>::ThreadSafeBlockingQueue(uint32_t capacity) : elements(capacity), mutex(new Mutex()) {

}

template<class T>
ThreadSafeBlockingQueue<T>::~ThreadSafeBlockingQueue() {
    delete mutex;
}

template<class T>
void ThreadSafeBlockingQueue<T>::push(const T &element) {
    mutex->acquire();

    elements.add(element);

    mutex->release();
}

template<class T>
T ThreadSafeBlockingQueue<T>::pop() {

    while (isEmpty()) {}

    mutex->acquire();

    T ret = elements.remove((uint32_t) 0);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::add(const T &element) {
    mutex->acquire();

    bool ret = elements.add(element);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::addAll(const Collection<T> &other) {
    mutex->acquire();

    bool ret = elements.addAll(other);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::remove(const T &element) {
    mutex->acquire();

    bool ret = elements.remove(element);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::removeAll(const Collection<T> &other) {
    mutex->acquire();

    bool ret = elements.removeAll(other);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::contains(const T &element) const {
    mutex->acquire();

    bool ret = elements.contains(element);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::containsAll(const Collection<T> &other) const {
    mutex->acquire();

    bool ret =  elements.containsAll(other);

    mutex->release();

    return ret;
}

template<class T>
bool ThreadSafeBlockingQueue<T>::isEmpty() const {
    mutex->acquire();

    bool ret =  elements.isEmpty();

    mutex->release();

    return ret;
}

template<class T>
void ThreadSafeBlockingQueue<T>::clear() {
    mutex->acquire();

    elements.clear();

    mutex->release();
}

template<class T>
Iterator<T> ThreadSafeBlockingQueue<T>::begin() const {
    mutex->acquire();

    Iterator<T> ret = elements.begin();

    mutex->release();

    return ret;
}

template<class T>
Iterator<T> ThreadSafeBlockingQueue<T>::end() const {
    mutex->acquire();

    Iterator<T> ret = elements.end();

    mutex->release();

    return ret;
}

template<class T>
uint32_t ThreadSafeBlockingQueue<T>::size() const {
    mutex->acquire();

    uint32_t ret = elements.size();

    mutex->release();

    return ret;
}

template<class T>
Array<T> ThreadSafeBlockingQueue<T>::toArray() const {
    mutex->acquire();

    Array<T> ret = elements.toArray();

    mutex->release();

    return ret;
}
}


#endif

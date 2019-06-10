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

#ifndef __WorkerThread_include__
#define __WorkerThread_include__

#include "kernel/thread/Thread.h"

/**
 * @author Filip Krakowski
 */
template <typename T, typename R>
class WorkerThread : public Thread {

public:

    WorkerThread(R (*work)(const T&), const T &data, void (*callback)(const Thread&, const R&));

    void run() override;

private:

    R (*workFunction)(const T&);

    void (*onFinished)(const Thread&, const R&);

    T data;
};

template <class T, class R>
WorkerThread<T, R>::WorkerThread(R (*work)(const T&), const T &data, void (*callback)(const Thread&, const R&)) {

    this->workFunction = work;

    this->onFinished = callback;

    this->data = data;
}

template <class T, class R>
void WorkerThread<T, R>::run() {

    R result = workFunction(data);

    if (onFinished != nullptr) {

        onFinished(*this, result);
    }
}

#endif

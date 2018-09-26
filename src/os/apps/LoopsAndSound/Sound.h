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

#ifndef __Sound_include__
#define __Sound_include__

#include "kernel/threads/Thread.h"
#include "devices/sound/PcSpeaker.h"

/**
 * A simple thread, which plays a melody in an infinite loop.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2016, 2018
 */
class Sound : public Thread {

private:

    PcSpeaker *speaker = nullptr;

    TimeService * timeService = nullptr;

    bool isRunning = true;

public:

    /**
     * Constructor.
     */
    explicit Sound();

    /**
     * Copy-constructor.
     */
    Sound(const Sound &copy) = delete;

    /**
     * Destructor.
     */
    ~Sound() override;

    /**
     * Overriding function from Thread.
     */
    void run() override;
};

#endif

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

#ifndef HHUOS_LOOPSANDSOUND_H
#define HHUOS_LOOPSANDSOUND_H

#include "kernel/thread/Thread.h"
#include "kernel/event/Receiver.h"
#include "kernel/service/EventBus.h"

/**
 * A simple application, that demonstrates the multi-threading capabilities of hhuOS by showing two counters, while
 * simultaneously playing melodies on the speaker.
 */
class LoopsAndSound : public Kernel::Thread, Kernel::Receiver {

private:

    Kernel::EventBus *eventBus = nullptr;

    bool isRunning = true;

public:

    /**
     * Constructor.
     */
    LoopsAndSound();

    /**
     * Copy-constructor.
     */
    LoopsAndSound(const LoopsAndSound &copy) = delete;

    /**
     * Destructor.
     */
    ~LoopsAndSound() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Kernel::Event &event) override;

    /**
     * Overriding function from Thread.
     */
    void run() override;
};

#endif

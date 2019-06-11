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

#include "kernel/core/System.h"
#include "kernel/event/input/KeyEvent.h"
#include "application/loop/Loop.h"
#include "kernel/thread/Scheduler.h"
#include "application/Application.h"
#include "LoopsAndSound.h"
#include "Sound.h"

LoopsAndSound::LoopsAndSound() {
    eventBus = Kernel::System::getService<Kernel::EventBus>();
}

LoopsAndSound::~LoopsAndSound() {
    eventBus->unsubscribe(*this, Kernel::KeyEvent::TYPE);
}

void LoopsAndSound::onEvent(const Kernel::Event &event) {
    if(event.getType() == Kernel::KeyEvent::TYPE) {
        auto &keyEvent = (Kernel::KeyEvent&) event;

        if(keyEvent.getKey().scancode() == Kernel::KeyEvent::ESCAPE) {
            isRunning = false;
        }
    }
}

void LoopsAndSound::run() {
    eventBus->subscribe(*this, Kernel::KeyEvent::TYPE);

    Kernel::Thread *thread1 = new Loop(1, 1);
    Kernel::Thread *thread2 = new Loop(2, 3);
    Kernel::Thread *thread3 = new Sound();

    thread1->start();
    thread2->start();
    thread3->start();

    while(isRunning);

    Kernel::Scheduler::getInstance().kill(*thread1);
    Kernel::Scheduler::getInstance().kill(*thread2);
    Kernel::Scheduler::getInstance().kill(*thread3);

    delete thread1;
    delete thread2;
    delete thread3;

    eventBus->unsubscribe(*this, Kernel::KeyEvent::TYPE);
}

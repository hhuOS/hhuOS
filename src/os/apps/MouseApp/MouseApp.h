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

#ifndef HHUOS_MOUSEAPP_H
#define HHUOS_MOUSEAPP_H


#include <apps/Application.h>
#include <kernel/services/EventBus.h>
#include <kernel/Kernel.h>
#include <kernel/events/input/MouseClickedEvent.h>
#include <kernel/events/input/MouseReleasedEvent.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/services/GraphicsService.h>

class MouseApp : public Thread, Receiver {

private:
    int32_t xPos;
    int32_t yPos;

    Color color;
    bool isRunning = true;

    LinearFrameBuffer *lfb;
    EventBus *eventBus = nullptr;

public:
    MouseApp(const MouseApp &copy) = delete;

    MouseApp() : Thread("MouseApp"), Receiver() {
        eventBus = Kernel::getService<EventBus>();

        lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
        xPos = lfb->getResX() / 2 - 25/2;
        yPos = lfb->getResY() / 2 - 25/2;
        color = Colors::WHITE;
    }

    ~MouseApp() override {
        eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
    }

    void onEvent(const Event &event) override;

    void run() override;

};


#endif

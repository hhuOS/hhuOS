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

#include <lib/libc/printf.h>
#include <devices/input/Mouse.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/threads/Scheduler.h>
#include "MouseApp.h"

void MouseApp::onEvent(const Event &event) {
    if(event.getType() == KeyEvent::TYPE) {
        auto &keyEvent = (KeyEvent&) event;

        if(keyEvent.getKey().scancode() == KeyEvent::ESCAPE) {
            isRunning = false;
        }
    }

    if(event.getType() == MouseMovedEvent::TYPE) {
        MouseMovedEvent movedEvent = (MouseMovedEvent&) event;

        int32_t dx = movedEvent.getXMovement();
        int32_t dy = movedEvent.getYMovement();

        xPos += dx;
        if(xPos < 0) {
            xPos = 0;
        } else if(xPos + 25 >= lfb->getResX()) {
            xPos = lfb->getResX() - 25;
        }

        yPos += dy;
        if(yPos < 0) {
            yPos = 0;
        } else if(yPos + 25 >= lfb->getResY()) {
            yPos = lfb->getResY() - 25;
        }

    }

    if(event.getType() == MouseClickedEvent::TYPE) {
        MouseClickedEvent releasedEvent = (MouseClickedEvent&) event;
        if(releasedEvent.isLeftClicked()) {
            color = Colors::RED;
        }
        if(releasedEvent.isMiddleClicked()) {
            color = Colors::GREEN;
        }
        if(releasedEvent.isRightClicked()) {
            color = Colors::BLUE;
        }
    }

    if(event.getType() == MouseReleasedEvent::TYPE) {
        color = Colors::WHITE;
    }

    if(event.getType() == MouseDoubleClickEvent::TYPE) {
        color = Colors::BLACK;
    }

}


void MouseApp::run() {
    eventBus->subscribe(*this, KeyEvent::TYPE);
    eventBus->subscribe(*this, MouseMovedEvent::TYPE);
    eventBus->subscribe(*this, MouseClickedEvent::TYPE);
    eventBus->subscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->subscribe(*this, MouseDoubleClickEvent::TYPE);

    lfb->enableDoubleBuffering();

    while(isRunning) {
        lfb->fillRect(xPos, yPos, 25, 25, color);

        lfb->show();
    }


    eventBus->unsubscribe(*this, KeyEvent::TYPE);
    eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseDoubleClickEvent::TYPE);
    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}

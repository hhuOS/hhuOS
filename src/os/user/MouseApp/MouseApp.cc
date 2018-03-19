//
// Created by burak on 05.03.18.
//

#include <lib/libc/printf.h>
#include <devices/input/Mouse.h>
#include "MouseApp.h"

void MouseApp::onEvent(const Event &event) {
    if(event.getType() == MouseMovedEvent::TYPE) {
        MouseMovedEvent movedEvent = (MouseMovedEvent&) event;

        int32_t dx = movedEvent.getXMovement();
        int32_t dy = movedEvent.getYMovement();

        xPos += dx;
        if(xPos < 5) {
            xPos = 5;
        } else if(xPos + 30 >= lfb->getResX()) {
            xPos = lfb->getResX() - 30;
        }

        yPos += dy;
        if(yPos < 5) {
            yPos = 5;
        } else if(yPos + 30 >= lfb->getResY()) {
            yPos = lfb->getResY() - 30;
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
    eventBus->subscribe(*this, MouseMovedEvent::TYPE);
    eventBus->subscribe(*this, MouseClickedEvent::TYPE);
    eventBus->subscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->subscribe(*this, MouseDoubleClickEvent::TYPE);

    lfb->enableDoubleBuffering();

    while(isRunning) {
        lfb->fillRect(xPos, yPos, 25, 25, color);

        lfb->show();
    }
}

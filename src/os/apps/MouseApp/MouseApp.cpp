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
#include <kernel/threads/Scheduler.h>
#include "MouseApp.h"

MouseApp::MouseApp() : Thread("MouseApp"), Receiver() {

    File *file = File::open("/initrd/os/logo_v3.bmp", "r");
    logo = new Bmp(file);
    delete file;

    file = File::open("/initrd/icons/mouse_default.bmp", "r");
    mouseDefault = new Bmp(file);
    delete file;

    file = File::open("/initrd/icons/mouse_left_click.bmp", "r");
    mouseLeftClick = new Bmp(file);
    delete file;

    file = File::open("/initrd/icons/mouse_right_click.bmp", "r");
    mouseRightClick = new Bmp(file);
    delete file;

    file = File::open("/initrd/icons/mouse_scroll.bmp", "r");
    mouseScroll = new Bmp(file);
    delete file;

    currentIcon = mouseDefault;

    eventBus = Kernel::getService<EventBus>();
    timeService = Kernel::getService<TimeService>();

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
    xPos = lfb->getResX() / 2 - 25/2;
    yPos = lfb->getResY() / 2 - 25/2;
}

MouseApp::~MouseApp() {
    eventBus->unsubscribe(*this, KeyEvent::TYPE);
    eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseDoubleClickEvent::TYPE);
}

void MouseApp::onEvent(const Event &event) {
    if(event.getType() == KeyEvent::TYPE) {
        auto &keyEvent = (KeyEvent&) event;

        if(keyEvent.getKey().scancode() == KeyEvent::ESCAPE) {
            isRunning = false;
        }
    } else if(event.getType() == MouseMovedEvent::TYPE) {
        MouseMovedEvent movedEvent = (MouseMovedEvent&) event;

        int32_t dx = movedEvent.getXMovement();
        int32_t dy = movedEvent.getYMovement();

        xPos += dx;
        if(xPos < 0) {
            xPos = 0;
        } else if(xPos + mouseDefault->getWidth() >= lfb->getResX()) {
            xPos = lfb->getResX() - mouseDefault->getWidth();
        }

        yPos += dy;
        if(yPos < 0) {
            yPos = 0;
        } else if(yPos + mouseDefault->getHeight() >= lfb->getResY()) {
            yPos = lfb->getResY() - mouseDefault->getHeight();
        }

    } else if(event.getType() == MouseClickedEvent::TYPE) {
        MouseClickedEvent clickedEvent = (MouseClickedEvent&) event;

        if(clickedEvent.isLeftClicked()) {
            currentIcon = mouseLeftClick;
        }

        if(clickedEvent.isMiddleClicked()) {
            currentIcon = mouseScroll;
        }

        if(clickedEvent.isRightClicked()) {
            currentIcon = mouseRightClick;
        }
    } else if(event.getType() == MouseReleasedEvent::TYPE) {
        currentIcon = mouseDefault;
    } else if(event.getType() == MouseDoubleClickEvent::TYPE) {
        versionColor = Color(static_cast<uint8_t>(random.rand(255)), static_cast<uint8_t>(random.rand(255)),
                             static_cast<uint8_t>(random.rand(255)));
    }

}

void MouseApp::update() {
    Rtc::Date date = timeService->getCurrentDate();

    time = String::format("%02u.%02u.%04u %02u:%02u:%02u", date.dayOfMonth, date.month, date.year,
                          date.hours, date.minutes, date.seconds);
}

void MouseApp::drawScreen() {
    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_BLUE_30);

    logo->draw(static_cast<uint16_t>((lfb->getResX() - logo->getWidth()) / 2),
               static_cast<uint16_t>((lfb->getResY() - logo->getHeight()) / 2));

    lfb->placeString(std_font_8x16, 50, 15, (const char*) time, Colors::HHU_GRAY);
    lfb->placeString(sun_font_12x22, 50, 85, (const char*) version, versionColor);
    lfb->placeString(std_font_8x8, 25, 99, credits, Colors::HHU_LIGHT_GRAY);

    currentIcon->draw(static_cast<uint16_t>(xPos), static_cast<uint16_t>(yPos));

    lfb->show();
}

void MouseApp::run() {
    eventBus->subscribe(*this, KeyEvent::TYPE);
    eventBus->subscribe(*this, MouseMovedEvent::TYPE);
    eventBus->subscribe(*this, MouseClickedEvent::TYPE);
    eventBus->subscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->subscribe(*this, MouseDoubleClickEvent::TYPE);

    lfb->enableDoubleBuffering();

    float currentTime = timeService->getSystemTime() / 1000.0f;
    float acc = 0.0f;
    float delta = 0.01667f; // 60Hz

    while (isRunning) {
        float newTime = timeService->getSystemTime() / 1000.0f;
        float frameTime = newTime - currentTime;
        if(frameTime > 0.25f)
            frameTime = 0.25f;
        currentTime = newTime;

        acc += frameTime;

        while(acc >= delta){
            update();
            acc -= delta;
        }

        drawScreen();
    }

    eventBus->unsubscribe(*this, KeyEvent::TYPE);
    eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
    eventBus->unsubscribe(*this, MouseDoubleClickEvent::TYPE);
    Application::getInstance().resume();
    Scheduler::getInstance().exit();
}

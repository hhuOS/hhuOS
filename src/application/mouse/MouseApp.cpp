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

#include "lib/libc/printf.h"
#include "device/input/Mouse.h"
#include "kernel/thread/Scheduler.h"
#include "MouseApp.h"

MouseApp::MouseApp() : Thread("MouseApp"), Receiver() {

    logo = Bmp::load("/initrd/os/logo_v3.bmp");
    mouseDefault = Bmp::load("/initrd/icons/mouse_default.bmp");
    mouseLeftClick = Bmp::load("/initrd/icons/mouse_left_click.bmp");
    mouseRightClick = Bmp::load("/initrd/icons/mouse_right_click.bmp");
    mouseScroll = Bmp::load("/initrd/icons/mouse_scroll.bmp");

    currentIcon = mouseDefault;

    eventBus = Kernel::System::getService<Kernel::EventBus>();
    timeService = Kernel::System::getService<Kernel::TimeService>();

    lfb = Kernel::System::getService<Kernel::GraphicsService>()->getLinearFrameBuffer();
    xPos = lfb->getResX() / 2 - 25/2;
    yPos = lfb->getResY() / 2 - 25/2;
}

MouseApp::~MouseApp() {
    eventBus->unsubscribe(*this, Kernel::KeyEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseMovedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseClickedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseReleasedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseDoubleClickedEvent::TYPE);

    delete logo;
    delete mouseDefault;
    delete mouseLeftClick;
    delete mouseRightClick;
    delete mouseScroll;
}

void MouseApp::onEvent(const Kernel::Event &event) {
    if(event.getType() == Kernel::KeyEvent::TYPE) {
        auto &keyEvent = (Kernel::KeyEvent&) event;

        if(keyEvent.getKey().scancode() == Kernel::KeyEvent::ESCAPE) {
            isRunning = false;
        }
    } else if(event.getType() == Kernel::MouseMovedEvent::TYPE) {
        Kernel::MouseMovedEvent movedEvent = (Kernel::MouseMovedEvent&) event;

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

    } else if(event.getType() == Kernel::MouseClickedEvent::TYPE) {
        Kernel::MouseClickedEvent clickedEvent = (Kernel::MouseClickedEvent&) event;

        if(clickedEvent.isLeftClicked()) {
            currentIcon = mouseLeftClick;
        }

        if(clickedEvent.isMiddleClicked()) {
            currentIcon = mouseScroll;
        }

        if(clickedEvent.isRightClicked()) {
            currentIcon = mouseRightClick;
        }
    } else if(event.getType() == Kernel::MouseReleasedEvent::TYPE) {
        currentIcon = mouseDefault;
    } else if(event.getType() == Kernel::MouseDoubleClickedEvent::TYPE) {
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
    eventBus->subscribe(*this, Kernel::KeyEvent::TYPE);
    eventBus->subscribe(*this, Kernel::MouseMovedEvent::TYPE);
    eventBus->subscribe(*this, Kernel::MouseClickedEvent::TYPE);
    eventBus->subscribe(*this, Kernel::MouseReleasedEvent::TYPE);
    eventBus->subscribe(*this, Kernel::MouseDoubleClickedEvent::TYPE);

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

    eventBus->unsubscribe(*this, Kernel::KeyEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseMovedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseClickedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseReleasedEvent::TYPE);
    eventBus->unsubscribe(*this, Kernel::MouseDoubleClickedEvent::TYPE);
}

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
#include <kernel/events/input/MouseDoubleClickEvent.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/GraphicsService.h>
#include <lib/file/bmp/Bmp.h>
#include <BuildConfig.h>

class MouseApp : public Thread, Receiver {

private:
    int32_t xPos;
    int32_t yPos;

    Bmp *logo;

    Bmp *mouseDefault;
    Bmp *mouseLeftClick;
    Bmp *mouseRightClick;
    Bmp *mouseScroll;

    Bmp *currentIcon;

    Random random;

    bool isRunning = true;

    LinearFrameBuffer *lfb;
    EventBus *eventBus = nullptr;

    String version = String::format("hhuOS version %s", BuildConfig::VERSION);
    Color versionColor = Color(static_cast<uint8_t>(random.rand(255)), static_cast<uint8_t>(random.rand(255)),
                               static_cast<uint8_t>(random.rand(255)));

    static const constexpr char *credits = "Icons by Icons8 (https://www.icons8.com)";

public:
    MouseApp(const MouseApp &copy) = delete;

    MouseApp() : Thread("MouseApp"), Receiver() {

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

        lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
        xPos = lfb->getResX() / 2 - 25/2;
        yPos = lfb->getResY() / 2 - 25/2;
    }

    ~MouseApp() override {
        eventBus->unsubscribe(*this, KeyEvent::TYPE);
        eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseDoubleClickEvent::TYPE);
    }

    void drawScreen();

    void onEvent(const Event &event) override;

    void run() override;

};


#endif

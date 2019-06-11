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

#include "lib/string/String.h"
#include "lib/graphic/Screenshot.h"
#include "lib/async/WorkerThread.h"
#include "lib/file/FileStatus.h"
#include "kernel/event/input/KeyEvent.h"
#include "ScreenshotService.h"

namespace Kernel {

uint32_t ScreenshotService::screenshotCounter = 0;

ScreenshotService::ScreenshotService() {
    System::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);
}

ScreenshotService::~ScreenshotService() {
    System::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
}

void ScreenshotService::takeScreenshot() {
    Screenshot screenshot;

    screenshot.take();

    if (!FileStatus::exists("/screenshots")) {
        System::getService<Filesystem>()->createDirectory("/screenshots");
    }

    while (FileStatus::exists(String::format("/screenshots/screenshot%u.bmp", screenshotCounter))) {
        screenshotCounter++;
    }

    screenshot.saveToBmp(String::format("/screenshots/screenshot%u.bmp", screenshotCounter));
}

void ScreenshotService::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent &) event;

    if (keyEvent.getKey().scancode() == KeyEvent::PRINT) {
        takeScreenshot();
    }
}

}
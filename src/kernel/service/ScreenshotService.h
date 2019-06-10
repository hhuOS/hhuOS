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

#ifndef HHUOS_SCREENSHOTSERVICE_H
#define HHUOS_SCREENSHOTSERVICE_H

#include "KernelService.h"
#include "kernel/event/Receiver.h"

class ScreenshotService : public KernelService, public Receiver {

private:

    static uint32_t screenshotCounter;

public:

    ScreenshotService();

    ~ScreenshotService() override;

    void takeScreenshot();

    void onEvent(const Event &event) override;

    static constexpr const char* SERVICE_NAME = "ScreenshotService";
};

#endif

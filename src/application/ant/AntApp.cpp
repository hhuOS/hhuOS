/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, Jochen Peters
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

#include "device/time/Pit.h"
#include "kernel/event/input/KeyEvent.h"
#include "kernel/thread/Scheduler.h"
#include "application/Application.h"
#include "application/ant/AntApp.h"

void AntApp::run() {
    auto *eventBus = Kernel::System::getService<Kernel::EventBus>();
    eventBus->subscribe(*this, Kernel::KeyEvent::TYPE);

	Color color;

	while (isRunning) {
		lfb->readPixel(x, y, color);
		if ((color.getRed() + color.getGreen() + color.getBlue()) > 0) { // is white
			rotateRight();
			lfb->drawPixel(x, y, Colors::BLACK);
		} else {
			rotateLeft();
			lfb->drawPixel(x, y, _col);
		}
		forward();
	}

    eventBus->unsubscribe(*this, Kernel::KeyEvent::TYPE);
}

void AntApp::onEvent(const Kernel::Event &event) {

	auto &keyEvent = (Kernel::KeyEvent&) event;

	if(keyEvent.getKey().scancode() == Kernel::KeyEvent::ESCAPE) {
		isRunning = false;
	}
}
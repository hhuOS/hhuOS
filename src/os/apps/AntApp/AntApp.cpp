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

#include <devices/timer/Pit.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/threads/Scheduler.h>
#include <apps/Application.h>
#include "apps/AntApp/AntApp.h"

void AntApp::run() {
    auto *eventBus = Kernel::getService<EventBus>();
    eventBus->subscribe(*this, KeyEvent::TYPE);

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

    eventBus->unsubscribe(*this, KeyEvent::TYPE);
	Application::getInstance()->resume();
	Scheduler::getInstance()->exit();
}

void AntApp::onEvent(const Event &event) {

	auto &keyEvent = (KeyEvent&) event;

	if(keyEvent.getKey().scancode() == KeyEvent::ESCAPE) {
		isRunning = false;
	}
}
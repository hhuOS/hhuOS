#include <devices/Pit.h>
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
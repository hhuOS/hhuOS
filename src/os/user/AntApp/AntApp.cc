#include <devices/Pit.h>
#include "user/AntApp/AntApp.h"

void AntApp::run() {
	Color color;

	while (true) {
		lfb->readPixel(x, y, color);
		if ((color.getRed() + color.getGreen() + color.getBlue()) > 0) { // is white
			rotateRight();
			lfb->drawPixel(x, y, Colors::BLACK);
		} else {
			rotateLeft();
			lfb->drawPixel(x, y, _col);
		}
		forward();
		//Scheduler::me().yield();
	}
}

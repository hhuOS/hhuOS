#include "doomgeneric.h"
#include "doomkeys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"



int main(int argc, char **argv) {
	doomgeneric_Create(argc, argv);
	while (1) {
		doomgeneric_Tick();
	}
	return 0;
}


Util::Graphic::LinearFrameBuffer * lfb;
Util::Io::KeyDecoder * kd;


void DG_Init() {
	printf("DG_Init\n");
	
	Util::Graphic::Ansi::prepareGraphicalApplication(true);
	auto file = new Util::Io::File("/device/lfb") ;
	lfb = new Util::Graphic::LinearFrameBuffer( *file);
	
	kd = new Util::Io::KeyDecoder(new Util::Io::DeLayout());	
	
}

void DG_DrawFrame() {
	//printf("DG_DrawFrame\n");
	Util::Address<uint32_t> buf = lfb->getBuffer();
	
	for (int i=0; i<DOOMGENERIC_RESY; i++) {
		buf.copyRange(Util::Address<uint32_t>(DG_ScreenBuffer + DOOMGENERIC_RESX * i), DOOMGENERIC_RESX * 4);
		buf = buf.add(lfb->getResolutionX() * 4);
	}
}

void DG_SleepMs(uint32_t ms) {
	Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(ms));
}

uint32_t DG_GetTicksMs() {
	return clock();
}

int DG_GetKey(int * pressed, unsigned char * key) {
	if (!stdin->isReadyToRead()) {
		return 0;
		
	} else {
		//todo: fire, lalt;
		uint8_t scancode = fgetc(stdin);
		
		if ((scancode & ~0x80) == 0x1d) {
			*pressed = !(scancode & 0x80);
			*key = KEY_FIRE;
			return 1;
		}
		if ((scancode & ~0x80) == 0x38) {
			*pressed = !(scancode & 0x80);
			*key = KEY_LALT;
			return 1;
		}
		
		
		if (kd->parseScancode(scancode)) {
			auto k = kd->getCurrentKey();
			if (!k.isValid()) return 0;
			
			*pressed = k.isPressed() ? 1:0; 
			
			if (k.getScancode() >= 0x3b && k.getScancode() <= 0x44) { // handle F1-10
				*key = k.getScancode() + 0x80;
				return 1;
			}
			
			switch(k.getScancode()) {
				case Util::Io::Key::UP:
					*key = KEY_UPARROW;
					return 1;
				case Util::Io::Key::DOWN:
					*key = KEY_DOWNARROW;
					return 1;
				case Util::Io::Key::LEFT:
					*key = KEY_LEFTARROW;
					return 1;
				case Util::Io::Key::RIGHT:
					*key = KEY_RIGHTARROW;
					return 1;
				case Util::Io::Key::SPACE:
					*key = KEY_USE;
					return 1;
				case Util::Io::Key::ESC:
					*key = KEY_ESCAPE;
					return 1;
				case Util::Io::Key::ENTER:
					*key = KEY_ENTER;
					return 1;
				case Util::Io::Key::TAB:
					*key = KEY_TAB;
					return 1;
				case Util::Io::Key::BACKSPACE:
					*key = KEY_BACKSPACE;
					return 1;
				default:
					if (k.getAscii()) *key = tolower(k.getAscii());
					else return 0;
					return 1;
				
			}
		}
		
	}
	return 0;
}

void DG_SetWindowTitle(const char * title) {
}
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

#ifndef __AntApp_include__
#define __AntApp_include__

#include "kernel/event/Receiver.h"
#include <cstdint>
#include "kernel/thread/Thread.h"

#include "application/ant/Limits.h"
#include "lib/math/Random.h"
#include "lib/graphic/Color.h"
#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "kernel/core/Kernel.h"

/**
 * Langstons Ant.
 * This is a vesa 2D turing machine Thread.
 *
 * @author Jochen Peters, Fabian Ruhland, Filip Krakowski
 * @date 2017
 */
class AntApp : public Thread, public Receiver {

private:
	Random * _random;
	AntApp (const AntApp &copy);
	uint8_t ori;
	Limits _lim{};
	Color    _col;
	bool   _bouncing;
	bool isRunning = true;
	LinearFrameBuffer *lfb = nullptr;

	struct DI {
		enum {
			up=1,
			right=2,
			down=3,
			left=4
		};
	};

public:
	int x;
	int y;

	AntApp () : Thread ("ant") {
		lfb = (Kernel::getService<GraphicsService>())->getLinearFrameBuffer();
		_bouncing = true;
		_random = new Random(400);
		_col = Colors::YELLOW;
		_lim.left   = 0;
		_lim.right  = lfb->getResX() -1;
		_lim.top    = 0;
		_lim.bottom = lfb->getResY() -1;
		ori = DI::down;
		x = _lim.left + (_lim.right-_lim.left)/2;
		y = _lim.top  + (_lim.bottom-_lim.top)/2;
	}
	
	/**
	 * Set the Playground.
	 * Set the limits for the ant.
	 * @param left first x position
	 * @param right last x position
	 * @param top first y position
	 * @param bottom last y position
	 */
	void set(int left, int right, int top, int bottom) {
		_lim.left   = left;
		_lim.right  = right;
		_lim.top    = top;
		_lim.bottom = bottom;
		x = _lim.left + (_lim.right-_lim.left)/2;
		y = _lim.top  + (_lim.bottom-_lim.top)/2;
	}

	void rotateLeft() {
		ori--; if(ori==0) ori=4;
	}

	void rotateRight() {
		ori++; if(ori==5) ori=1;
	}

	/**
	 * forward.
	 * it sets the ant to the next field.
	 */
	void forward() {
		if (ori == DI::up) {
			y--;
		} else if (ori == DI::right) {
			x++;
		} else if (ori == DI::left) {
			x--;
		} else {
			y++;
		}
		/*
		if (_bouncing) {
			if (x < _lim.left) {
				ori = DI::right;
			} else if (x >= _lim.right) {
				ori = DI::left;
			}
			if (y < _lim.top) {
				ori = DI::down;
			} else if (y >= _lim.bottom) {
				ori = DI::up;
			}
			_lim.likeBorder(x,y);
		*/
		if (_bouncing) {
			if (x < _lim.left) {
				x = _random->rand();
				y = _random->rand();
				_col = Color(127 + _random->rand()%128, 127 + _random->rand()%128, 127 + _random->rand()%128);
			} else if (x >= _lim.right) {
				x = _random->rand();
				y = _random->rand();
				_col = Color(127 + _random->rand()%128, 127 + _random->rand()%128, 127 + _random->rand()%128);
			}
			if (y < _lim.top) {
				x = _random->rand();
				y = _random->rand();
				_col = Color(127 + _random->rand()%128, 127 + _random->rand()%128, 127 + _random->rand()%128);
			} else if (y >= _lim.bottom) {
				x = _random->rand();
				y = _random->rand();
				_col = Color(127 + _random->rand()%128, 127 + _random->rand()%128, 127 + _random->rand()%128);
			}
			_lim.likeBorder(x,y);
		} else {
			_lim.likeWorld(x,y);
		}
	}
	
	void run();

	void onEvent(const Event &event) override;
	
	char * name(){ return "ant";}
};

#endif

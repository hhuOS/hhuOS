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

#include <kernel/Kernel.h>
#include <kernel/services/TimeService.h>
#include "lib/Random.h"



Random::Random(uint32_t seed, uint32_t maxRand) {
	this->seed = seed;
	this->maxRand = maxRand;
}

Random::Random(uint32_t maxRand) {
	this->seed = (Kernel::getService<TimeService>())->getSystemTime();
	this->maxRand = maxRand;
}


// Liefert eine Zufallszahl zwischen 0 und maxRand - 1
unsigned int Random::rand() {
	this->seed = this->seed * 1103515255 + 12345;
	return (unsigned int)(seed / 65536) % maxRand;
}

unsigned int Random::rand(unsigned int maxRand) {
	this->seed = this->seed * 1103515255 + 12345;
	return (unsigned int)(seed / 65536) % maxRand;
}

void Random::setSeed(uint32_t seed) {
	this->seed = seed;
}

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

#ifndef __Random_include__
#define __Random_include__

#include <cstdint>

class Random {

private:
	Random(const Random &copy);

	uint32_t seed;
	uint32_t maxRand; // Obere Grenze für die Zufallszahl

public:
	// Konstruktoren
	Random(uint32_t seed, uint32_t maxRand);
	Random(uint32_t maxRand = 0xFFFFFFFF);

	// Liefert eine Zufallszahl zwischen 0 und maxRand - 1
	uint32_t rand();
	uint32_t rand(uint32_t maxRand);

    void setSeed(uint32_t seed);
};

#endif

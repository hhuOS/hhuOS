/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HHUEngine_include__
#define __HHUEngine_include__


#include "lib/Random.h"
#include "apps/game/Game.h"
#include <devices/sound/PcSpeaker.h>

/**
 * @author Filip Krakowski
 */
class HHUEngine {

  private:

    static Random random;

    static Game* currentGame;

  public:

    static bool isKeyPressed(int scancode);

    static unsigned long time();

    static void beep(float frequency = PcSpeaker::C0, uint32_t length = 100);

    static float rand();

    static void setSeed(unsigned int seed);

    static void setCurrentGame(Game* game);

    static bool strEqual(char* s1, char* s2);

    static int strLen(char* string);

    static bool isGameSet();

    static void instantiate(GameObject* gameObject);

    static void destroy(GameObject* gameObject);

};

#endif

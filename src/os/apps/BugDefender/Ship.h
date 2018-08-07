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

#ifndef __Ship_include__
#define __Ship_include__

#include <lib/graphic/Bmp.h>
#include "apps/game/GameObject.h"
#include "apps/BugDefender/Missile.h"

class Ship : public GameObject {

  private:

    Ship(const Ship &copy); // Verhindere Kopieren

    const static int width = 40;
    const static int height = 33;

    constexpr static float speed = 200.0f;

    const static int waitTime = 300;

    unsigned long lastShootTime;

    int points;

    Bmp *bitmap;
    
  public:

    Ship(Vector2 position);

    ~Ship(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void onCollisionEnter(GameObject &other) override;

    void onCollisionExit();

};

#endif

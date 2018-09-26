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

#ifndef __Enemy_include__
#define __Enemy_include__

#include "apps/game/GameObject.h"
#include "apps/BugDefender/Fleet.h"
#include "apps/BugDefender/Missile.h"

class Enemy : public GameObject {

  private:

    Enemy(const Enemy &copy); // Verhindere Kopieren

    static Image *sprite1;
    static Image *sprite2;

    const static int width = 30;
    const static int height = 20;

    int spriteNumber;

    float lifeTime;

    int rowNumber;

  public:

    Enemy(Vector2 position, int rowNumber);

    ~Enemy(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void onCollisionEnter(GameObject &other) override;

    void onCollisionExit();

    void shoot();

};

#endif

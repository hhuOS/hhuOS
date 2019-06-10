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

#ifndef __GameObject_include__
#define __GameObject_include__

#include "Vector2.h"
#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "RectCollider.h"

/**
 * @author Filip Krakowski
 */
class GameObject {

  protected:

    Vector2 position;

    char *tag;

  public:

    RectCollider* collider = nullptr;

    bool colliding = false;

    GameObject(Vector2 position, char* tag);

    virtual ~GameObject();

    virtual void update(float delta) = 0;

    virtual void draw(LinearFrameBuffer* g2d) = 0;

    virtual void onCollisionEnter(GameObject &other);

    virtual void onCollisionExit();

    Vector2 getPosition();

    char* getTag();

    void move(float x, float y);

    void setPosition(float x, float y);

    void setPosition(Vector2 position);

};

#endif

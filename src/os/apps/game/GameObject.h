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

#include "apps/game/Vector2.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "apps/game/RectCollider.h"

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

    GameObject(Vector2 position, char* tag) : position(position), tag(tag) {}

    virtual ~GameObject(){
        if (collider) {
            delete collider;
        }
    }

    virtual void update(float delta) = 0;

    virtual void draw(LinearFrameBuffer* g2d) = 0;

    virtual void onCollisionEnter(GameObject &other){};

    virtual void onCollisionExit(){};

    Vector2 getPosition() {
      return position;
    }

    char* getTag() {
      return tag;
    }

    void move(float x, float y){
      this->position.set(position.getX() + x, position.getY() + y);

        if (collider) {
            collider->setPosition(position.getX() + x, position.getY() + y);
        }
    }

    void setPosition(float x, float y){
      this->position.set(x, y);
    }

    void setPosition(Vector2 position){
      this->position = position;
    }

};

#endif

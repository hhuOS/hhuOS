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

#ifndef __RectCollider_include__
#define __RectCollider_include__

#include "Vector2.h"

/**
 * @author Filip Krakowski
 */
class RectCollider {

  private:

    Vector2 position;
    int width;
    int height;

    RectCollider(const RectCollider &copy);

  public:

    RectCollider (Vector2 position, int width, int heigth);

    bool isColliding(RectCollider &other);

    void setPosition(float x, float y);

    void setPosition(Vector2 position);


};

#endif

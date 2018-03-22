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

#include "user/game/RectCollider.h"

RectCollider::RectCollider(Vector2 position, int width, int height) : position(position), width(width), height(height) {}

bool RectCollider::isColliding(RectCollider &other){
  return position.getX() < other.position.getX() + other.width &&
         position.getX() + width > other.position.getX() &&
         position.getY() < other.position.getY() + other.height &&
         position.getY() + height > other.position.getY();
}

void RectCollider::setPosition(float x, float y){
  this->position.set(x, y);
}

void RectCollider::setPosition(Vector2 position){
  this->position = position;
}

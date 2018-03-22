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

#include "user/BugDefender/Missile.h"
#include "user/game/HHUEngine.h"

Missile::Missile(Vector2 position, int direction, char* tag) : GameObject(position, tag) {
  collider = new RectCollider(position, width, height);
  this->direction = direction;

}

void Missile::update(float delta){
  move(0, -speed * delta * direction);

  if(position.getY() < -height || position.getY() > 435){
    HHUEngine::destroy(this);
  }
}

void Missile::draw(LinearFrameBuffer* g2d){
  if(direction == 1)
    g2d->drawRect(position.getX(), position.getY(), width, height, Color(255,255,255));
  else
    g2d->drawRect(position.getX(), position.getY(), width, height, Color(255,10,10));
}

void Missile::onCollisionEnter(GameObject &other){
  if(direction == -1 && HHUEngine::strEqual(other.getTag(), "Enemy"))
    return;

  HHUEngine::destroy(this);
}

void Missile::onCollisionExit(){

}

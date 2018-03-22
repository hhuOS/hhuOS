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

#include "user/BugDefender/Ship.h"
#include "user/game/HHUEngine.h"
#include "BugDefender.h"

Ship::Ship(Vector2 position) : GameObject(position, "Player") {
  collider = new RectCollider(position, width, height);
  lastShootTime = 0;
}

void Ship::update(float delta){
  if(HHUEngine::isKeyPressed(75)){
    if(position.getX() > 0)
      move(-speed * delta, 0);
  } else if(HHUEngine::isKeyPressed(77)) {
    if(position.getX() < 640 - width)
      move(speed * delta, 0);
  }

  if(HHUEngine::isKeyPressed(57) && lastShootTime + waitTime < HHUEngine::time()){
    HHUEngine::instantiate(new Missile(Vector2(position.getX() + (width / 2), position.getY() - 20), 1, "PlayerMissile"));
    lastShootTime = HHUEngine::time();
  }
}

void Ship::draw(LinearFrameBuffer* g2d){
  g2d->drawRect(position.getX(), position.getY(), width, height, Color(10,255,10));
  g2d->drawRect(position.getX() + (width / 2) - 2, position.getY() - 5, 4, 5, Color(10,255,10));
}

void Ship::onCollisionEnter(GameObject &other){
  BugDefender::takeDamage(1);
}

void Ship::onCollisionExit(){

}

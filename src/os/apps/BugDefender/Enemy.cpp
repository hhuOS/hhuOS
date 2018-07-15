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

#include "apps/BugDefender/Enemy.h"
#include "apps/game/HHUEngine.h"
#include "BugDefender.h"
#include "apps/BugDefender/sprites/bug_1.cpp"
#include "apps/BugDefender/sprites/bug_2.cpp"

Enemy::Enemy(Vector2 position, int rowNumber) : GameObject(position, "Enemy") {
  collider = new RectCollider(position, width, height);
  spriteNumber = 0;
  lifeTime = 0.0f;
  this->rowNumber = rowNumber;
}

void Enemy::update(float delta){
  // Halte Lebenszeit fest, um den richtigen Sprite zur richtigen Zeit anzuzeigen
  lifeTime += delta;
  spriteNumber = ((int) lifeTime) % 2;

  move(Fleet::fleetSpeed * delta, Fleet::isMovingDown() ? height : 0);

  if(position.getX() < 0 || position.getX() > 640 - width){
    Fleet::invertSpeed();
    Fleet::moveDown();
  }

  if(position.getY() > 415)
    BugDefender::gameOver();

  if(HHUEngine::rand() > 0.999f)
    shoot();
}

void Enemy::draw(LinearFrameBuffer* g2d){
    switch (spriteNumber) {
        case 0:
            g2d->drawSprite(position.getX(), position.getY(), bug_1.width, bug_1.height, (int*) &bug_1.pixel_data[0]);
            break;
        case 1:
            g2d->drawSprite(position.getX(), position.getY(), bug_2.width, bug_2.height, (int*) &bug_2.pixel_data[0]);
            break;
    }
}

void Enemy::onCollisionEnter(GameObject &other){
  if( !HHUEngine::strEqual(other.getTag(), "PlayerMissile") )
    return;

  HHUEngine::destroy(this);

  // Erhoehe Geschwindigkeit aller Schiffe, falls eines zerstoert wird
  Fleet::increaseSpeed();

  // Jede zweite Reihe gibt mehr Punkte
  BugDefender::addPoints(( 4 - (rowNumber / 2)) * 20);

  BugDefender::enemyShot();

  HHUEngine::beep(Speaker::C2);
}

void Enemy::onCollisionExit(){

}

void Enemy::shoot(){
  HHUEngine::instantiate(new Missile(Vector2(position.getX() + (width / 2), position.getY() + 20), -1, "EnemyMissile"));
}

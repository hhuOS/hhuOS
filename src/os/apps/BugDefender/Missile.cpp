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

#include "apps/BugDefender/Missile.h"
#include "apps/game/HHUEngine.h"


Bmp *Missile::enemySprite = nullptr;
Bmp *Missile::shipSprite = nullptr;

Missile::Missile(Vector2 position, int direction, char* tag) : GameObject(position, tag) {
    collider = new RectCollider(position, width, height);
    this->direction = direction;

    if (enemySprite == nullptr) {
        enemySprite = new Bmp(File::open("/initrd/game/res/laserRed.bmp", "r"));
    }

    if (shipSprite == nullptr) {
        shipSprite = new Bmp(File::open("/initrd/game/res/laserGreen.bmp", "r"));
    }
}

void Missile::update(float delta){
    move(0, -speed * delta * direction);

    if(position.getY() < -height || position.getY() > 435){
        HHUEngine::destroy(this);
    }
}

void Missile::draw(LinearFrameBuffer* g2d){
    if(direction == 1) {
        shipSprite->draw(static_cast<uint16_t>(position.getX()), static_cast<uint16_t>(position.getY()));
    } else {
        enemySprite->draw(static_cast<uint16_t>(position.getX()), static_cast<uint16_t>(position.getY()));
    }
}

void Missile::onCollisionEnter(GameObject &other){
    if(direction == -1 && HHUEngine::strEqual(other.getTag(), "Enemy"))
        return;

    HHUEngine::destroy(this);
}

void Missile::onCollisionExit(){

}

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

#include "user/BugDefender/Fleet.h"

float Fleet::fleetSpeed = 10.0f;
float Fleet::nextSpeed = 10.0f;
bool Fleet::speedInverted = false;
bool Fleet::speedIncreased = false;
int Fleet::moveDownCounter = 0;

void Fleet::invertSpeed(){
  if(speedInverted)
    return;

  nextSpeed *= -1;
  speedInverted = true;
}

void Fleet::increaseSpeed(){
  if(speedIncreased)
    return;

  nextSpeed *= 1.05;
  speedIncreased = true;
}

void Fleet::swapSpeed(){

  if(moveDownCounter > 0)
    moveDownCounter--;

  if(!speedInverted && !speedIncreased)
    return;

  fleetSpeed = nextSpeed;
  speedInverted = false;
  speedIncreased = false;

}

void Fleet::moveDown(){
  moveDownCounter = 2;
}

bool Fleet::isMovingDown(){
  return moveDownCounter == 1;
}

void Fleet::reset(){
  fleetSpeed = 10.0f;
  nextSpeed = 10.0f;
  speedInverted = false;
  speedIncreased = false;
  moveDownCounter = 0;
}

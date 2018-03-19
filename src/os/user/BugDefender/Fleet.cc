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

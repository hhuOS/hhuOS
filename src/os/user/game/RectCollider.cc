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

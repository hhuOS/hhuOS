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

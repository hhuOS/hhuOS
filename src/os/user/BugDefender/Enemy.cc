#include "user/BugDefender/Enemy.h"
#include "user/game/HHUEngine.h"
#include "BugDefender.h"
#include "user/BugDefender/sprites/bug_1.cc"
#include "user/BugDefender/sprites/bug_2.cc"

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
}

void Enemy::onCollisionExit(){

}

void Enemy::shoot(){
  HHUEngine::instantiate(new Missile(Vector2(position.getX() + (width / 2), position.getY() + 20), -1, "EnemyMissile"));
}

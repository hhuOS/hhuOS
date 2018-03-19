#ifndef __GameObject_include__
#define __GameObject_include__

#include "user/game/Vector2.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "user/game/RectCollider.h"

/**
 * @author Filip Krakowski
 */
class GameObject {

  protected:

    Vector2 position;

    char *tag;

  public:

    RectCollider* collider = 0x0;

    bool colliding = false;

    GameObject(Vector2 position, char* tag) : position(position), tag(tag) {}

    virtual ~GameObject(){
        if (collider) {
            delete collider;
        }
    }

    virtual void update(float delta) = 0;

    virtual void draw(LinearFrameBuffer* g2d) = 0;

    virtual void onCollisionEnter(GameObject &other){};

    virtual void onCollisionExit(){};

    Vector2 getPosition() {
      return position;
    }

    char* getTag() {
      return tag;
    }

    void move(float x, float y){
      this->position.set(position.getX() + x, position.getY() + y);

        if (collider) {
            collider->setPosition(position.getX() + x, position.getY() + y);
        }
    }

    void setPosition(float x, float y){
      this->position.set(x, y);
    }

    void setPosition(Vector2 position){
      this->position = position;
    }

};

#endif

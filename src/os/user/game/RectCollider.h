#ifndef __RectCollider_include__
#define __RectCollider_include__

#include "user/game/Vector2.h"

/**
 * @author Filip Krakowski
 */
class RectCollider {

  private:

    Vector2 position;
    int width;
    int height;

    RectCollider(const RectCollider &copy); // Verhindere Kopieren

  public:

    RectCollider (Vector2 position, int width, int heigth);

    bool isColliding(RectCollider &other);

    void setPosition(float x, float y);

    void setPosition(Vector2 position);


};

#endif

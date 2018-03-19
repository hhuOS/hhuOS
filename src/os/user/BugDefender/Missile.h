#ifndef __Missile_include__
#define __Missile_include__

#include "user/game/GameObject.h"

class Missile : public GameObject {

  private:

    Missile(const Missile &copy); // Verhindere Kopieren

    const static int width = 4;
    const static int height = 8;

    constexpr static float speed = 200.0f;

    int direction;

  public:

    Missile(Vector2 position, int direction, char* tag);

    ~Missile(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void onCollisionEnter(GameObject &other) override;

    void onCollisionExit();

};

#endif

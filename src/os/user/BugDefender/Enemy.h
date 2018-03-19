#ifndef __Enemy_include__
#define __Enemy_include__

#include "user/game/GameObject.h"
#include "user/BugDefender/Fleet.h"
#include "user/BugDefender/Missile.h"

class Enemy : public GameObject {

  private:

    Enemy(const Enemy &copy); // Verhindere Kopieren

    const static int width = 30;
    const static int height = 20;

    int spriteNumber;

    float lifeTime;

    int rowNumber;

  public:

    Enemy(Vector2 position, int rowNumber);

    ~Enemy(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void onCollisionEnter(GameObject &other) override;

    void onCollisionExit();

    void shoot();

};

#endif

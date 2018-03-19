#ifndef __Ship_include__
#define __Ship_include__

#include "user/game/GameObject.h"
#include "user/BugDefender/Missile.h"

class Ship : public GameObject {

  private:

    Ship(const Ship &copy); // Verhindere Kopieren

    const static int width = 40;
    const static int height = 8;

    constexpr static float speed = 200.0f;

    const static int waitTime = 30;

    unsigned long lastShootTime;

    int points;
    
  public:

    Ship(Vector2 position);

    ~Ship(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void onCollisionEnter(GameObject &other) override;

    void onCollisionExit();

};

#endif

#ifndef __Invaders_include__
#define __Invaders_include__

#include "user/game/HHUEngine.h"
#include "user/game/GameObject.h"
#include "user/BugDefender/Ship.h"
#include "user/BugDefender/Enemy.h"
#include "user/BugDefender/Fleet.h"
#include "lib/deprecated/Queue.h"
#include "user/game/Game.h"

class BugDefender : public Game {

  private:

    BugDefender(const BugDefender &copy); // Verhindere Kopieren

    const static int enemiesPerLine = 11;

    const static int enemyLines = 6;

    static int enemyCount;

    static int score;

    static int lifes;

    bool isMenuVisible;

    static bool isGameOver;

    static bool isGameWon;

  public:

    BugDefender();

    ~BugDefender(){}

    void update(float delta);

    void draw(LinearFrameBuffer* g2d);

    void checkCollisions(GameObject* o);

    void drawInfo(LinearFrameBuffer* g2d);

    void showMenu(LinearFrameBuffer* g2d);

    void showGameOver(LinearFrameBuffer* g2d);

    void showGameWon(LinearFrameBuffer* g2d);

    int getPoints();

    static void addPoints(int points);

    static void takeDamage(int amount);

    static void gameOver();

    static void enemyShot();
};

#endif

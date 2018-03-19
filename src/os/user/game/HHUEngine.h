#ifndef __HHUEngine_include__
#define __HHUEngine_include__


#include "lib/Random.h"
#include "user/game/Game.h"

/**
 * @author Filip Krakowski
 */
class HHUEngine {

  private:

    static Random random;

    static Game* currentGame;

  public:

    static bool isKeyPressed(int scancode);

    static void invalidateInput();

    static unsigned long time();

    static void beep();

    static float rand();

    static void setSeed(unsigned int seed);

    static void setCurrentGame(Game* game);

    static bool strEqual(char* s1, char* s2);

    static int strLen(char* string);

    static bool isGameSet();

    static void instantiate(GameObject* gameObject);

    static void destroy(GameObject* gameObject);

};

#endif

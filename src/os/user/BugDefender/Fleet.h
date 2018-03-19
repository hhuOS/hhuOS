#ifndef __Fleet_include__
#define __Fleet_include__

class Fleet {

  private:

    Fleet(const Fleet &copy); // Verhindere Kopieren

    static float nextSpeed;

    static bool speedInverted;

    static bool speedIncreased;

    static int moveDownCounter;

  public:

    static float fleetSpeed;

    static void invertSpeed();

    static void increaseSpeed();

    static void swapSpeed();

    static void moveDown();

    static bool isMovingDown();

    static void reset();

};

#endif

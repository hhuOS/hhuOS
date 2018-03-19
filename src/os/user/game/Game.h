#ifndef __Game_include__
#define __Game_include__

#include <lib/deprecated/ArrayList.h>
#include "user/game/Vector2.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "user/game/RectCollider.h"
#include "lib/deprecated/Queue.h"
#include "user/game/GameObject.h"

/**
 * @author Filip Krakowski
 */
class Game {

  private:

    ArrayList<GameObject> *removeList;

    ArrayList<GameObject> *addList;

  protected:

    ArrayList<GameObject> *gameObjects;

  public:

    Game(){
      removeList = new ArrayList<GameObject>();
      addList = new ArrayList<GameObject>();
      gameObjects = new ArrayList<GameObject>();
      isRunning = true;
    };

    virtual ~Game(){
      delete removeList;
      delete addList;
      delete gameObjects;
    }

    bool isRunning;

    virtual void update(float delta) = 0;

    virtual void draw(LinearFrameBuffer* g2d) = 0;

    void addGameObject(GameObject &gameObject) {
      addList->add(gameObject);
    }

    void removeGameObject(GameObject &gameObject) {
      removeList->add(gameObject);
    }

    void applyChanges(){

      GameObject *go;
      for (uint32_t i = 0; i < addList->size(); i++) {
          go = addList->get(i);
          gameObjects->add(*go);
      }

      for (uint32_t i = 0; i < removeList->size(); i++) {
        go = removeList->get(i);
        gameObjects->remove(*go);
      }

      removeList->clear();
      addList->clear();
    }

};

#endif

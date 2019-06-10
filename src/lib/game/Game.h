/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __Game_include__
#define __Game_include__

#include "lib/util/ArrayList.h"
#include "Vector2.h"
#include "device/graphic/lfb/LinearFrameBuffer.h"
#include "RectCollider.h"
#include "GameObject.h"

/**
 * @author Filip Krakowski
 */
class Game {

  private:

    Util::ArrayList<GameObject*> *removeList;

    Util::ArrayList<GameObject*> *addList;

  protected:

    Util::ArrayList<GameObject*> *gameObjects;

  public:

    Game();

    virtual ~Game();

    bool isRunning;

    virtual void update(float delta) = 0;

    virtual void draw(LinearFrameBuffer* g2d) = 0;

    void addGameObject(GameObject &gameObject);

    void removeGameObject(GameObject &gameObject);

    void applyChanges();

};

#endif

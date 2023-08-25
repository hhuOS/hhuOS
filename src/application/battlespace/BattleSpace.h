/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_BATTLESPACE_H
#define HHUOS_BATTLESPACE_H

#include "lib/util/game/3d/Scene.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/math/Random.h"
#include "Player.h"

class BattleSpace : public Util::Game::D3::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    BattleSpace();

    /**
     * Copy Constructor.
     */
    BattleSpace(const BattleSpace &other) = delete;

    /**
     * Assignment operator.
     */
    BattleSpace &operator=(const BattleSpace &other) = delete;

    /**
     * Destructor.
     */
    ~BattleSpace() override = default;

    double getRandomNumber();

private:

    void update(double delta) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

    void updateEntities(double delta) override;

    void fireMissile(const Util::Math::Vector3D &position, const Util::Math::Vector3D &direction, bool enemy);

private:

    Util::Math::Random random;

    Player *player = new Player();
    Util::Math::Vector3D inputRotation = {0, 0, 0};
    Util::Math::Vector3D inputTranslate = {0, 0, 0};
    double inputSpeed = 1.0;
    bool playerFireMissile = false;

    bool enemyAlive = false;
    uint16_t difficulty = 1;
};

#endif

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
#include "lib/util/game/MouseListener.h"
#include "lib/util/math/Random.h"
#include "Player.h"
#include "Enemy.h"

class BattleSpace : public Util::Game::D3::Scene, public Util::Game::KeyListener, public Util::Game::MouseListener {

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

    void update(double delta) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

    void buttonPressed(Util::Io::Mouse::Button button) override;

    void buttonReleased(Util::Io::Mouse::Button button) override;

    void mouseMoved(const Util::Math::Vector2D &relativeMovement) override;

    void mouseScrolled(Util::Io::Mouse::ScrollDirection direction) override;

private:

    Util::Math::Random random;

    Player *player = new Player(enemies);
    Util::ArrayList<Enemy*> enemies;

    Util::Math::Vector3D inputRotation = {0, 0, 0};
    Util::Math::Vector3D inputTranslate = {0, 0, 0};
    double inputSpeed = 1.0;
    uint16_t difficulty = 0;

    static const constexpr double ENEMY_SPAWN_RANGE = 10.0;
};

#endif

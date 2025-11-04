/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 *
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_H
#define HHUOS_ROGUE3D_H

#include <stdint.h>

#include "application/rogue3d/entities/Room.h"
#include "lib/pulsar/3d/Scene.h"
#include "lib/util/math/Random.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Vector3.h"

class Hud;
class Player;

class Rogue3D : public Pulsar::D3::Scene {

public:
    /**
     * Default Constructor.
     */
    Rogue3D();

    /**
     * Constructor.
     */
    explicit Rogue3D(Player *player);

    /**
     * Copy Constructor.
     */
    Rogue3D(const Rogue3D &other) = delete;

    /**
     * Assignment operator.
     */
    Rogue3D &operator=(const Rogue3D &other) = delete;

    /**
     * Destructor.
     */
    ~Rogue3D() override = default;

    void initialize() override;

    void swapRooms(Util::Math::Vector3<double> &newPosition, double &roomCenterX, double &roomCenterZ);

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    void generateRooms();

    void setAdjacentRooms() const;

    void generateLevel();

    [[nodiscard]] Room& getRoomByPos(uint32_t row, uint32_t column) const;
    
    Player *player = nullptr;
    Hud *hud = nullptr;
    Room *currentRoom = nullptr;

    Util::Math::Random random;
    Room::Type levelGen[4][4]{};
    Util::ArrayList<Room*> rooms;

    Util::Math::Vector3<double> inputTranslate = Util::Math::Vector3<double>(0, 0, 0);
};

#endif

/*
* Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef DEMOMODEL_H
#define DEMOMODEL_H

#include "lib/util/game/3d/Model.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/math/Vector3.h"

class DemoModel : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    DemoModel(const Util::String &modelPath, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale, const Util::Graphic::Color &color);

    /**
     * Copy Constructor.
     */
    DemoModel(const DemoModel &other) = delete;

    /**
     * Assignment operator.
     */
    DemoModel &operator=(const DemoModel &other) = delete;

    /**
     * Destructor.
     */
    ~DemoModel() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;
};



#endif

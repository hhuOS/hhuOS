/*
* Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include <stdint.h>

#include "lib/util/game/3d/Model.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

class DemoModel : public Util::Game::D3::Model {

public:
    enum Type {
        TREE, LANTERN, ICOSPHERE
    };

    /**
     * Constructor.
     */
    DemoModel(Type type, const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, const Util::Math::Vector3<double> &scale, const Util::Graphic::Color &color = Util::Graphic::Colors::WHITE);

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

    void initialize() override;

    void draw(Util::Game::Graphics &graphics) override;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

private:

    Type type;
    uint32_t drawListID = UINT32_MAX;
    Util::Graphic::Color color;

    static const char* pathForType(Type type);

    static uint32_t TREE_DRAW_LIST_ID;
    static uint32_t LANTERN_DRAW_LIST_ID;
    static uint32_t ICOSPHERE_DRAW_LIST_ID;
};



#endif

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
 * view3d has originally been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_VIEW3D_MODEL_H
#define HHUOS_VIEW3D_MODEL_H

#include <stdint.h>

#include "lib/util/pulsar/3d/Model.h"
#include "lib/util/base/String.h"

class ModelEntity : public Util::Pulsar::D3::Model {

public:
    /**
     * Constructor.
     */
    explicit ModelEntity(const Util::String &modelPath);

    /**
     * Constructor.
     */
    explicit ModelEntity(const Util::String &modelPath, const Util::String &texturePath);

    /**
     * Copy Constructor.
     */
    ModelEntity(const ModelEntity &other) = delete;

    /**
     * Assignment operator.
     */
    ModelEntity &operator=(const ModelEntity &other) = delete;

    /**
     * Destructor.
     */
    ~ModelEntity() override = default;

    void initialize() override;

    void draw(Util::Pulsar::Graphics &graphics) const override;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Pulsar::D3::CollisionEvent &event) override;

private:

    uint32_t drawListID = UINT32_MAX;
};

#endif

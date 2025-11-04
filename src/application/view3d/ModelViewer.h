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

#ifndef HHUOS_MODELVIEWER_H
#define HHUOS_MODELVIEWER_H

#include "lib/util/pulsar/3d/Scene.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3.h"

class ModelEntity;

namespace Util {
namespace Pulsar {
namespace D3 {
class Light;
}  // namespace D3
}  // namespace Pulsar
}  // namespace Util

class ModelViewer : public Util::Pulsar::D3::Scene {

public:
    /**
     * Constructor.
     */
    explicit ModelViewer(const Util::String &path);

    /**
     * Copy Constructor.
     */
    ModelViewer(const ModelViewer &other) = delete;

    /**
     * Assignment operator.
     */
    ModelViewer &operator=(const ModelViewer &other) = delete;

    /**
     * Destructor.
     */
    ~ModelViewer() override = default;

    void initialize() override;

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    Util::String modelPath;
    ModelEntity *model = nullptr;
    Util::Pulsar::D3::Light *light = nullptr;

    Util::Math::Vector3<double> modelRotation = Util::Math::Vector3<double>(0, 0, 0);
    Util::Math::Vector3<double> cameraRotation = Util::Math::Vector3<double>(0, 0, 0);
    Util::Math::Vector3<double> cameraTranslation = Util::Math::Vector3<double>(0, 0, 0);
};


#endif

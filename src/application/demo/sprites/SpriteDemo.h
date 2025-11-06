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
 */

#ifndef HHUOS_SPRITEDEMO_H
#define HHUOS_SPRITEDEMO_H

#include <stdint.h>

#include "lib/pulsar/2d/Scene.h"
#include "DemoSpriteFactory.h"
#include "lib/util/collection/ArrayListQueue.h"

class DemoSprite;

class SpriteDemo : public Pulsar::D2::Scene {

public:
    /**
     * Constructor.
     */
    explicit SpriteDemo(uint32_t initialCount);

    /**
     * Copy Constructor.
     */
    SpriteDemo(const SpriteDemo &other) = delete;

    /**
     * Assignment operator.
     */
    SpriteDemo &operator=(const SpriteDemo &other) = delete;

    /**
     * Destructor.
     */
    ~SpriteDemo() override = default;

    void initialize() override;

    void update(double delta) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    uint32_t initialCount;
    DemoSpriteFactory factory;
    Util::ArrayListQueue<DemoSprite*> entities;
};

#endif

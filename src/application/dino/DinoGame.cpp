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

#include "DinoGame.h"

#include "application/dino/Dino.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/entity/component/GravityComponent.h"
#include "Tree.h"

void DinoGame::update(double delta) {
    if (getObjectCount() == 0) {
        dino = new Dino();
        ground = new Ground(Util::Math::Vector2D(dino->getPosition().getX() - 1, -0.8));
        pointText = new Util::Game::Text(Util::Math::Vector2D(-1, 0.9), "Points: 0");

        dino->addComponent(new Util::Game::LinearMovementComponent(*dino));
        dino->addComponent(new Util::Game::GravityComponent(*dino, 2, 0.0025));

        addObject(dino);
        addObject(ground);
        addObject(pointText);

        setKeyListener(*this);
    }

    if (dino->hasHatched()) {
        dino->moveRight();
        dino->setTargetVelocity(dino->getTargetVelocity() + delta / 100);
        if (dino->getVelocity().getX() > 1) {
            dino->dash(true);
        }

        if (treeCooldown <= 0) {
            auto rand = treeRandom.nextRandomNumber();
            if (rand * delta >= 0.0158) {
                auto *tree = new Tree(Util::Math::Vector2D(getCamera().getPosition().getX() + 2, ground->getPosition().getY()));
                trees.add(tree);
                addObject(tree);
                treeCooldown = 1.5;
            }
        } else {
            treeCooldown -= delta;
        }
    } else {
        for (auto *tree : trees) {
            trees.remove(tree);
            removeObject(tree);
        }
    }

    for (auto *tree : trees) {
        if (tree->getPosition().getX() < getCamera().getPosition().getX() - 2) {
            trees.remove(tree);
            removeObject(tree);
        }
    }

    getCamera().setPosition(Util::Math::Vector2D(dino->getPosition().getX() + 0.8, 0));
    ground->setPosition(Util::Math::Vector2D(dino->getPosition().getX() - 1, ground->getPosition().getY()));
    pointText->setText(Util::String::format("Points: %u", static_cast<uint32_t>(getCamera().getPosition().getX())));
}

void DinoGame::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            stop();
            break;
        case Util::Io::Key::SPACE :
            if (dino->isDead()) {
                dino->reset();
            } else if (dino->hasHatched()) {
                dino->jump();
            } else {
                dino->hatch();
            }
            break;
    }
}

void DinoGame::keyReleased(Util::Io::Key key) {}
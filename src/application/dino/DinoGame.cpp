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
#include "Saw.h"

DinoGame::DinoGame() : ground(4) {}

void DinoGame::update(double delta) {
    if (getObjectCount() == 0) {
        dino = new Dino(Util::Math::Vector2D(-0.8, 0));
        pointText = new Util::Game::Text(Util::Math::Vector2D(-1, 0.9), "Points: 0");

        dino->addComponent(new Util::Game::LinearMovementComponent(*dino));
        dino->addComponent(new Util::Game::GravityComponent(*dino, 2, 0.0025));

        addObject(dino);
        addObject(pointText);

        for (uint32_t i = 0; i < 4; i++) {
            auto *newGround = new Ground(Util::Math::Vector2D(getCamera().getPosition().getX() - 1.5 + i, -0.8));
            ground.offer(newGround);
            addObject(newGround);
        }

        setKeyListener(*this);
    }

    if (dino->hasHatched() && (dino->isDying() == dino->isDead())) {
        if (currentVelocity < MAX_VELOCITY) {
            currentVelocity += (delta / 100);
            if (currentVelocity >= DASH_VELOCITY) {
                dino->dash();
            }
        }

        dino->setVelocityX(currentVelocity);

        if (obstacleCooldown <= 0) {
            if (random.nextRandomNumber() < delta) {
                auto positionY = ground.peek()->getPosition().getY() + (random.nextRandomNumber() < 0.5 ? 0 : + 0.3);
                auto *saw = new Saw(Util::Math::Vector2D(getCamera().getPosition().getX() + 2, positionY));
                obstacles.add(saw);
                addObject(saw);
                obstacleCooldown = OBSTACLE_COOLDOWN;
            }
        } else {
            obstacleCooldown -= delta;
        }

        if (ground.peek()->getPosition().getX() < getCamera().getPosition().getX() - 2.5) {
            auto positionX = (static_cast<uint32_t>((getCamera().getPosition().getX() + 1.5) * 10) / 5) * 5 / 10.0 ;
            auto *newGround = new Ground(Util::Math::Vector2D(positionX, -0.8));
            removeObject(ground.poll());
            ground.offer(newGround);
            addObject(newGround);
        }

        for (auto *tree : obstacles) {
            if (tree->getPosition().getX() < getCamera().getPosition().getX() - 2) {
                obstacles.remove(tree);
                removeObject(tree);
            }
        }

        getCamera().setPosition(Util::Math::Vector2D(dino->getPosition().getX() + 0.8, 0));
        pointText->setText(Util::String::format("Points: %u", static_cast<uint32_t>(getCamera().getPosition().getX())));
    }
}

void DinoGame::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            stop();
            break;
        case Util::Io::Key::SPACE :
            if (dino->isDead()) {
                dino->reset();
                dino->setPosition(Util::Math::Vector2D(-0.8, 0));
                getCamera().setPosition(Util::Math::Vector2D(dino->getPosition().getX() + 0.8, 0));
                currentVelocity = START_VELOCITY;
                pointText->setText("Points: 0");

                while (!ground.isEmpty()) {
                    removeObject(ground.poll());
                }

                for (auto *tree : obstacles) {
                    obstacles.remove(tree);
                    removeObject(tree);
                }

                for (uint32_t i = 0; i < 4; i++) {
                    auto *newGround = new Ground(Util::Math::Vector2D(getCamera().getPosition().getX() - 1.5 + i, -0.8));
                    ground.offer(newGround);
                    addObject(newGround);
                }
            } else if (dino->hasHatched()) {
                dino->jump();
            } else {
                dino->hatch();
            }
            break;
    }
}

void DinoGame::keyReleased(Util::Io::Key key) {}

#include "application/rogue3d/entities/Player.h"
#include "application/rogue3d/entities/Projectile.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Camera.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/math/Math.h"
#include "Rogue3D.h"
#include "application/rogue3d/entities/Hud.h"
#include "lib/util/base/Panic.h"
#include "lib/util/game/3d/Light.h"
#include "lib/util/graphic/Color.h"

Rogue3D::Rogue3D() : player(new Player) {}

Rogue3D::Rogue3D(Player *player) : player(player) {}

void Rogue3D::initialize() {
    generateLevel();
    generateRooms();

    // Setup camera
    camera.reset();
    camera.setPosition(Util::Math::Vector3<double>(player->getPosition().getX(), 34, player->getPosition().getZ() + 21));
    camera.setRotation(Util::Math::Vector3<double>(0, -60, 0));

    hud = new Hud(levelGen, *player);
    hud->setCurrentPosition(currentRoom->getRow(), currentRoom->getColumn());

    // Setup lighting
    setAmbientLight(Util::Graphic::Color(153, 153, 153));
    addLight(Util::Game::D3::Light::POINT, Util::Math::Vector3<double>(39, 20, 36), Util::Graphic::Color(102, 102, 255), Util::Graphic::Color(0, 0 ,0));
    setLightEnabled(true);

    addObject(player);
    addObject(hud);
    setKeyListener(*this);
}

void Rogue3D::generateRooms() {
    for (uint32_t row = 0; row < 4; row++) {
        for (uint32_t column = 0;column < 4; column++) {
            switch (levelGen[row][column]) {
                case Room::START: {
                    auto *room = new Room(Util::Math::Vector3<double>(26 * column, 0, 26 * row), Room::START, row, column);
                    room->generateEnemies(random, *player);
                    rooms.add(room);
                    currentRoom = room;
                    room->enterRoom();
                    addObject(room);
                    room->addEnemiesToScene();

                    player->setPosition(room->getPosition() + Util::Math::Vector3<double>(0, 1.5, 0));
                }
                break;
                case Room::END: {
                    auto *room = new Room(Util::Math::Vector3<double>(26 * column, 0, 26 * row), Room::END, row, column);
                    room->generateBoss(*player);
                    rooms.add(room);
                    addObject(room);
                    room->addEnemiesToScene();
                }
                break;
                case Room::ROOM: {
                    auto *room = new Room(Util::Math::Vector3<double>(26 * column, 0, 26 * row), Room::ROOM, row, column);
                    room->generateEnemies(random, *player);
                    rooms.add(room);
                    addObject(room);
                    room->addEnemiesToScene();
                }
                default:
                    break;
            }
        }
    }

    setAdjacentRooms();
}

void Rogue3D::setAdjacentRooms() const {
    for (uint32_t i = 0; i < rooms.size(); i++) {
        const auto row = rooms.get(i)->getRow();
        const auto column = rooms.get(i)->getColumn();

        if (row == 0 && column == 0) {
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
        } else if (row == 0 && column == 3) {
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
        } else if (row == 3 && column == 0) {
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        } else if (row == 3 && column == 3) {
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        } else if (row == 0) {
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
        } else if (row == 3) {
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        } else if (column == 0) {
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        } else if (column == 3) {
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        } else {
            if (levelGen[row][column + 1] != Room::NONE) rooms.get(i)->setRightRoom(getRoomByPos(row, column + 1)); // Check right
            if (levelGen[row][column - 1] != Room::NONE) rooms.get(i)->setLeftRoom(getRoomByPos(row, column - 1)); // Check left
            if (levelGen[row + 1][column] != Room::NONE) rooms.get(i)->setDownRoom(getRoomByPos(row + 1, column)); // Check down
            if (levelGen[row - 1][column] != Room::NONE) rooms.get(i)->setTopRoom(getRoomByPos(row - 1, column)); // Check up
        }
    }
}

Room& Rogue3D::getRoomByPos(uint32_t row, uint32_t column) const {
    uint32_t i = 0;
    while (i < rooms.size()) {
        const auto checkRow = rooms.get(i)->getRow();
        const auto checkColumn = rooms.get(i)->getColumn();
        if (checkRow == row && checkColumn == column) {
            return *rooms.get(i);
        }

        i++;
    }

    Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Rogue3D: Room not found!");
}

void Rogue3D::generateLevel() {
    int32_t startRow = 0;
    int32_t startColumn = 0;
    int32_t endRow = 0;
    int32_t endColumn = 0;

    while (startRow == endRow && startColumn == endColumn) {
        startRow = static_cast<int32_t>(random.nextRandomNumber() * 4);
        startColumn = static_cast<int32_t>(random.nextRandomNumber() * 4);
        endRow = static_cast<int32_t>(random.nextRandomNumber() * 4);
        endColumn = static_cast<int32_t>(random.nextRandomNumber() * 4);
    }

    levelGen[startRow][startColumn] = Room::START;
    levelGen[endRow][endColumn] = Room::END;

    // Generate rooms between start and end
    uint32_t placedX = 0;
    uint32_t placedY = 0;
    int32_t row = startRow;
    int32_t column = startColumn;

    while (Util::Math::absolute(startColumn - endColumn) > placedY) {
        if (startColumn < endColumn) {
            if (levelGen[row][column + 1] == Room::NONE) {
                levelGen[row][column + 1] = Room::ROOM;
            }

            column++;
            placedY++;
        } else if (startColumn > endColumn) {
            if (levelGen[row][column - 1] == Room::NONE) {
                levelGen[row][column - 1] = Room::ROOM;
            }

            column--;
            placedY++;
        }
    }

    while (Util::Math::absolute(startRow - endRow) > placedX) {
        if (startRow < endRow) {
            if (levelGen[row + 1][column] == Room::NONE) {
                levelGen[row + 1][column] = Room::ROOM;
            }

            row++;
            placedX++;
        } else if (startRow > endRow) {
            if (levelGen[row - 1][column] == Room::NONE) {
                levelGen[row - 1][column] = Room::ROOM;
            }

            row--;
            placedX++;
        }
    }
}

void Rogue3D::update(double delta) {
    // Player movement
    auto newPosition= player->getPosition() + (inputTranslate * delta * 10);
    double roomCenterX = currentRoom->getPosition().getX();
    double roomCenterZ = currentRoom->getPosition().getZ();

    if (currentRoom->isCleared()) {
        swapRooms(newPosition,roomCenterX, roomCenterZ);
        hud->setCurrentPosition(currentRoom->getRow(), currentRoom->getColumn());
    }

    if (newPosition.getX() > roomCenterX + 9.25) {
        newPosition = Util::Math::Vector3<double>(roomCenterX + 9.25, newPosition.getY(), newPosition.getZ());
    } else if (newPosition.getX() < roomCenterX - 9.25) {
        newPosition = Util::Math::Vector3<double>(roomCenterX - 9.25, newPosition.getY(), newPosition.getZ());
    } else if (newPosition.getZ() > roomCenterZ + 9.25) {
        newPosition = Util::Math::Vector3<double>(newPosition.getX(), newPosition.getY(), roomCenterZ + 9.25);
    } else if (newPosition.getZ() < roomCenterZ - 9.25) {
        newPosition = Util::Math::Vector3<double>(newPosition.getX(), newPosition.getY(), roomCenterZ - 9.25);
    }

    player->setPosition(newPosition);
    camera.setPosition(Util::Math::Vector3<double>(currentRoom->getPosition().getX(), 34, currentRoom->getPosition().getZ() + 21));
}

void Rogue3D::swapRooms(Util::Math::Vector3<double> &newPosition, double &roomCenterX, double &roomCenterZ) {
    // Go to Right room
    if (newPosition.getX() == roomCenterX + 9.25 && Util::Math::absolute(newPosition.getZ() - roomCenterZ) < 0.5) {
        if (currentRoom->hasRightRoom()) {
            currentRoom->leaveRoom();
            currentRoom = &currentRoom->getRightRoom();
            currentRoom->enterRoom();

            roomCenterX = currentRoom->getPosition().getX();
            roomCenterZ = currentRoom->getPosition().getZ();
            newPosition = Util::Math::Vector3<double>(roomCenterX - 8.50, newPosition.getY(), newPosition.getZ());
        }
    }

    // Go to Left room
    if (newPosition.getX() == roomCenterX - 9.25 && Util::Math::absolute(newPosition.getZ() - roomCenterZ) < 0.5){
        if (currentRoom->hasLeftRoom()) {
            currentRoom->leaveRoom();
            currentRoom = &currentRoom->getLeftRoom();
            currentRoom->enterRoom();

            roomCenterX = currentRoom->getPosition().getX();
            roomCenterZ = currentRoom->getPosition().getZ();
            newPosition= Util::Math::Vector3<double>(roomCenterX + 8.50, newPosition.getY(), newPosition.getZ());
        }
    }

    // Go to Bottom room
    if (newPosition.getZ() == roomCenterZ + 9.25 && Util::Math::absolute(newPosition.getX() - roomCenterX) < 0.5){
        if (currentRoom->hasDownRoom()) {
            currentRoom->leaveRoom();
            currentRoom = &currentRoom->getDownRoom();
            currentRoom->enterRoom();

            roomCenterX = currentRoom->getPosition().getX();
            roomCenterZ = currentRoom->getPosition().getZ();
            if (currentRoom->getType() == Room::END) {
                newPosition = Util::Math::Vector3<double>(newPosition.getX(), newPosition.getY(), roomCenterZ + 8.5);
            } else {
                newPosition = Util::Math::Vector3<double>(newPosition.getX(), newPosition.getY(), roomCenterZ - 8.5);
            }

        }
    }

    // Go to Top room
    if (newPosition.getZ() == roomCenterZ - 9.25 && Util::Math::absolute(newPosition.getX() - roomCenterX) < 0.5) {
        if (currentRoom->hasTopRoom()) {
            currentRoom->leaveRoom();
            currentRoom = &currentRoom->getTopRoom();
            currentRoom->enterRoom();

            roomCenterX = currentRoom->getPosition().getX();
            roomCenterZ = currentRoom->getPosition().getZ();
            newPosition = Util::Math::Vector3<double>(newPosition.getX(), newPosition.getY(), roomCenterZ + 8.5);
        }
    }
}

void Rogue3D::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::LEFT:
            if (player->shoot()){
                addObject(new Projectile(player->getPosition(),Util::Math::Vector3<double>(-1,0,0), Projectile::TAG_PLAYER));
            }
            break;
        case Util::Io::Key::RIGHT:
            if (player->shoot()){
                addObject(new Projectile(player->getPosition(),Util::Math::Vector3<double>(1,0,0), Projectile::TAG_PLAYER));
            }
            break;
        case Util::Io::Key::UP:
            if (player->shoot()){
                addObject(new Projectile(player->getPosition(),Util::Math::Vector3<double>(0,0,-1), Projectile::TAG_PLAYER));
            }
            break;
        case Util::Io::Key::DOWN:
            if (player->shoot()){
                addObject(new Projectile(player->getPosition(),Util::Math::Vector3<double>(0,0,1), Projectile::TAG_PLAYER));
            }
            break;
        case Util::Io::Key::W:
            inputTranslate = Util::Math::Vector3<double>(inputTranslate.getX(), inputTranslate.getY(), -1);
            break;
        case Util::Io::Key::S:
            inputTranslate = Util::Math::Vector3<double>(inputTranslate.getX(), inputTranslate.getY(), 1);
            break;
        case Util::Io::Key::A:
            inputTranslate = Util::Math::Vector3<double>(-1, inputTranslate.getY(), inputTranslate.getZ());
            break;
        case Util::Io::Key::D:
            inputTranslate = Util::Math::Vector3<double>(1, inputTranslate.getY(), inputTranslate.getZ());
            break;
        default:
            break;
    }
}

void Rogue3D::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::W:
        case Util::Io::Key::S:
            inputTranslate = Util::Math::Vector3<double>(inputTranslate.getX(), inputTranslate.getY(), 0);
            break;
        case Util::Io::Key::A:
        case Util::Io::Key::D:
            inputTranslate = Util::Math::Vector3<double>(0, inputTranslate.getY(), inputTranslate.getZ());
            break;
        default:
            break;
    }
}

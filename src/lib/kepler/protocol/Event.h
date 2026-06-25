/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_EVENT_H
#define HHUOS_EVENT_H

#include <stddef.h>
#include <stdint.h>

#include <util/async/Streamable.h>
#include <util/io/key/KeyEvent.h>

namespace Kepler {
namespace Event {

enum Type : uint8_t {
    MOUSE_HOVER,
    MOUSE_CLICK,
    KEY_EVENT,
    WINDOW_CLOSE
};

class BasicEvent : public Util::Async::Streamable {

public:

    BasicEvent() = default;

    explicit BasicEvent(const size_t windowId) : windowId(windowId) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    size_t getWindowId() const {
        return windowId;
    }

private:

    size_t windowId = 0;
};

class MouseHover final : public BasicEvent {

public:

    MouseHover() = default;

    MouseHover(const size_t windowId, const uint16_t posX, const uint16_t posY) :
        BasicEvent(windowId), posX(posX), posY(posY) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    uint16_t getPosX() const {
        return posX;
    }

    uint16_t getPosY() const {
        return posY;
    }

private:

    uint16_t posX = 0;
    uint16_t posY = 0;
};

class MouseClick final : public BasicEvent {

public:

    enum Button : uint8_t {
        LEFT,
        RIGHT,
        MIDDLE
    };

    enum Action : uint8_t {
        PRESS,
        RELEASE
    };

    MouseClick() = default;

    MouseClick(const size_t windowId, const uint16_t posX, const uint16_t posY, const Button button, const Action action) :
        BasicEvent(windowId), posX(posX), posY(posY), button(button), action(action) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    uint16_t getPosX() const {
        return posX;
    }

    uint16_t getPosY() const {
        return posY;
    }

    Button getButton() const {
        return button;
    }

    Action getAction() const {
        return action;
    }

private:

    uint16_t posX = 0;
    uint16_t posY = 0;
    Button button = LEFT;
    Action action = PRESS;
};

class KeyEvent final : public BasicEvent {

public:

    KeyEvent() = default;

    explicit KeyEvent(const size_t windowId, const Util::Io::KeyEvent &key) : BasicEvent(windowId), key(key) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;

    const Util::Io::KeyEvent& getKey() const {
        return key;
    }

private:

    Util::Io::KeyEvent key;
};

class WindowCloseEvent final : public BasicEvent {

public:

    WindowCloseEvent() = default;

    explicit WindowCloseEvent(const size_t windowId) : BasicEvent(windowId) {}

    bool writeToStream(Util::Io::OutputStream &stream) const override;

    bool readFromStream(Util::Io::InputStream &stream) override;
};

}
}

#endif

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

#ifndef HHUOS_WINDOWMANAGER_H
#define HHUOS_WINDOWMANAGER_H



#include "Client.h"
#include "ClientWindow.h"
#include "util/async/IdGenerator.h"
#include "util/async/Runnable.h"
#include "util/collection/ArrayList.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "util/time/Timestamp.h"

class WindowManager final : public Util::Async::Runnable {

public:

    explicit WindowManager(Util::Graphic::LinearFrameBuffer &lfb);

    void run() override;

private:

    void createNextPipe();

    void createWindow(Client &client);

    void flushWindow(const Client &client) const;

    size_t processId = Util::Async::Process::getCurrentProcess().getId();

    Util::Graphic::LinearFrameBuffer &lfb;
    Util::Graphic::BufferedLinearFrameBuffer doubleLfb;
    Util::Graphic::BufferedLinearFrameBuffer tripleLfb;
    bool needRedraw = false;

    int32_t mouseX = 0;
    int32_t mouseY = 0;

    Util::ArrayList<Client*> clients;
    Util::Async::IdGenerator clientIdGenerator;
    Util::Async::IdGenerator windowIdGenerator;

    Util::Io::FileInputStream *nextPipe = nullptr;
    size_t nextClientId = 0;

    static constexpr size_t TARGET_FPS = 60;
    static const Util::Time::Timestamp TARGET_FRAMETIME;
};

#endif
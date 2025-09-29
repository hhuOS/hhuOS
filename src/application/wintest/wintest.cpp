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


#include "application/desktop/Protocol.h"
#include "async/SharedMemory.h"
#include "lib/interface.h"
#include "base/System.h"
#include "graphic/Colors.h"
#include "graphic/LinearFrameBuffer.h"
#include "io/stream/FileInputStream.h"
#include "io/stream/FileOutputStream.h"
#include "io/file/File.h"

int32_t main() {
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));

    auto desktopFile = Util::Io::File("/system/desktop");
    if (!desktopFile.exists()) {
        Util::System::error << "Window manager not available!"
            << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;

        return -1;
    }

    auto desktopFileStream = Util::Io::FileInputStream(desktopFile);
    auto line = desktopFileStream.readLine();
    auto windowManagerId = Util::String::parseNumber<size_t>(line.content);
    auto processId = Util::Async::Process::getCurrentProcess().getId();

    createPipe("window-manager");
    auto *windowManagerInputPipe = new Util::Io::FileInputStream(Util::String::format("/process/%u/pipes/window-manager", processId));
    Util::Io::FileOutputStream *windowManagerOutputPipe = nullptr;

    auto windowManagerPipeDirectory = Util::Io::File(Util::String::format("/process/%u/pipes", windowManagerId));
    while (true) {
        size_t maxId = 0;
        for (const auto &child : windowManagerPipeDirectory.getChildren()) {
            const auto currentId = Util::String::parseNumber<size_t>(child.getName());
            if (currentId > maxId) {
                maxId = currentId;
            }
        }

        windowManagerOutputPipe = new Util::Io::FileOutputStream(Util::String::format("/process/%u/pipes/%u", windowManagerId, maxId));

        auto request = CreateWindowRequest{320, 240, processId, "window-manager"};
        if (request.writeToStream(*windowManagerOutputPipe)) {
            break;
        }
    }

    auto response = CreateWindowResponse::readFromStream(*windowManagerInputPipe);
    auto sharedMemory = Util::Async::SharedMemory(windowManagerId, Util::String::format("%u", response.sharedBufferId), response.sharedBufferPageCount);
    sharedMemory.map();

    auto *windowBuffer = reinterpret_cast<void*>(sharedMemory.getAddress().get());
    auto colorDepth = response.colorDepth == 15 ? 16 : response.colorDepth;
    auto lfb = Util::Graphic::LinearFrameBuffer(windowBuffer, response.resX, response.resY, response.colorDepth, response.resX * (colorDepth / 8));

    while (true) {
        lfb.fillSquare(10, 10, 100, Util::Graphic::Colors::HHU_BLUE);
        windowManagerOutputPipe->write(Command::FLUSH);

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));

        lfb.fillSquare(10, 10, 100, Util::Graphic::Colors::HHU_GREEN);
        windowManagerOutputPipe->write(Command::FLUSH);

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));
    }

    return 0;
}

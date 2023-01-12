/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>
#include <demos/benchmark/lv_demo_benchmark.h>
#include <music/lv_demo_music.h>
#include <src/core/lv_obj.h>
#include <src/hal/lv_hal_tick.h>
#include <src/misc/lv_timer.h>
#include <stress/lv_demo_stress.h>
#include <widgets/lv_demo_widgets.h>

#include "lib/util/file/File.h" // IWYU pragma: keep
#include "lib/util/stream/PrintWriter.h" // IWYU pragma: keep
#include "lib/util/graphic/Ansi.h" // IWYU pragma: keep
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/system/System.h"
#include "lib/util/ArgumentParser.h"
#include "lib/lvgl/LvglDriver.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Demo application for the 'Light and Versatile Graphics Library'.\n"
                               "Included demos are: 'benchmark', 'stress', 'widgets' and 'music' (Default: benchmark)\n"
                               "Usage: lvgl [DEMO]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    auto demo = Util::Memory::String(arguments.length() > 0 ? arguments[0] : "benchmark");

    auto lfbFile = Util::File::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto driver = LvglDriver(lfb);
    Util::Graphic::Ansi::prepareGraphicalApplication(false);

    lv_init();
    driver.initialize();

    if (demo == "stress") {
        lv_demo_stress();
    } else if (demo == "widgets") {
        lv_demo_widgets();
    } else if (demo == "music") {
        lv_demo_music();
    } else {
        lv_demo_benchmark_set_max_speed(true);
        lv_demo_benchmark();
    }

    while (driver.isRunning()) {
        auto time = Util::Time::getSystemTime().toMilliseconds();
        auto sleepTime = lv_timer_handler();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(sleepTime));
        lv_tick_inc(Util::Time::getSystemTime().toMilliseconds() - time);
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}
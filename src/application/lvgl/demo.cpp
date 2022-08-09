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
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/time/Timestamp.h"
#include "LvglDriver.h"
#include "lib/util/async/Thread.h"
#include "lib/util/system/System.h"

Util::Data::Array<Util::Memory::String> demos({"benchmark", "stress", "widgets", "music"});

int32_t main(int32_t argc, char *argv[]) {
    auto demo = Util::Memory::String(argc > 1 ? argv[1] : "benchmark");
    if (!demos.contains(demo)) {
        Util::System::error << "Invalid argument! Please specify the demo to run (benchmark/stress/widgets/music)" << Util::Stream::PrintWriter::endl;
        return -1;
    }

    lv_init();
    auto lfb = Util::Graphic::LinearFrameBuffer(Util::File::File("/device/lfb"));
    auto driver = LvglDriver(lfb);
    driver.initialize();

    if (demo == "benchmark") {
        lv_demo_benchmark_set_max_speed(true);
        lv_demo_benchmark();
    } else if (demo == "stress") {
        lv_demo_stress();
    } else if (demo == "widgets") {
        lv_demo_widgets();
    } else if (demo == "music") {
        lv_demo_music();
    }

    while (true) {
        auto time = Util::Time::getSystemTime().toMilliseconds();
        auto sleepTime = lv_timer_handler();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(sleepTime));
        lv_tick_inc(Util::Time::getSystemTime().toMilliseconds() - time);
    }
}
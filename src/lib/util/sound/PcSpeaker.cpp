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

#include "PcSpeaker.h"

#include "lib/util/async/Thread.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Io {
class File;
}  // namespace Io
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Util::Sound {

PcSpeaker::PcSpeaker(const Io::File &speakerFile) : stream(speakerFile) {}

void PcSpeaker::play(uint32_t frequency) {
    auto frequencyString = Util::String::format("%u", frequency);
    stream.write(static_cast<const uint8_t*>(frequencyString), 0, frequencyString.length());
}

void PcSpeaker::play(uint32_t frequency, const Time::Timestamp &length) {
    play(frequency);
    Util::Async::Thread::sleep(length);
}

void PcSpeaker::turnOff() {
    play(0);
}

}
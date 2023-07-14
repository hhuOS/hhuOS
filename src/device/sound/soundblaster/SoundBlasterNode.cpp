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

#include "SoundBlasterNode.h"

#include "lib/util/sound/SoundBlaster.h"
#include "device/sound/soundblaster/SoundBlaster.h"
#include "device/sound/soundblaster/SoundBlasterRunnable.h"
#include "kernel/process/Thread.h"
#include "lib/util/io/stream/PipedInputStream.h"
#include "lib/util/io/stream/PipedOutputStream.h"

namespace Device {

SoundBlasterNode::SoundBlasterNode(SoundBlaster *soundBlaster, SoundBlasterRunnable &runnable, Kernel::Thread &soundBlasterThread) :
    Filesystem::Memory::StreamNode("soundblaster", runnable.outputStream, runnable.inputStream), soundBlaster(soundBlaster), runnable(runnable), soundBlasterThread(soundBlasterThread) {}

SoundBlasterNode::~SoundBlasterNode() {
    runnable.stop();
    soundBlasterThread.join();
    delete soundBlaster;
}

bool SoundBlasterNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Sound::SoundBlaster::SET_AUDIO_PARAMETERS:
            if (parameters.length() < 3) {
                return false;
            }

            soundBlaster->setAudioParameters(parameters[0], parameters[1], parameters[2]);
            return true;
        default:
            return false;
    }
}

}
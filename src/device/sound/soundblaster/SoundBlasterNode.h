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

#ifndef HHUOS_SOUNDBLASTERNODE_H
#define HHUOS_SOUNDBLASTERNODE_H

#include "filesystem/memory/StreamNode.h"
#include "SoundBlaster.h"
#include "lib/util/io/stream/PipedInputStream.h"
#include "lib/util/io/stream/PipedOutputStream.h"
#include "SoundBlasterRunnable.h"
#include "kernel/process/Thread.h"

namespace Device {

class SoundBlasterNode : public Filesystem::Memory::StreamNode {

public:
    /**
     * Constructor.
     */
    explicit SoundBlasterNode(SoundBlaster *soundBlaster, SoundBlasterRunnable &runnable, Kernel::Thread &soundBlasterThread);

    /**
     * Copy Constructor.
     */
    SoundBlasterNode(const SoundBlasterNode &other) = delete;

    /**
     * Assignment operator.
     */
    SoundBlasterNode &operator=(const SoundBlasterNode &other) = delete;

    /**
     * Destructor.
     */
    ~SoundBlasterNode() override;

    /**
     * Overriding function from Node.
     */
    bool control(uint32_t request, const Util::Array<uint32_t> &parameters) override;

private:

    SoundBlaster *soundBlaster;
    SoundBlasterRunnable &runnable;
    Kernel::Thread &soundBlasterThread;
};

}

#endif

/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_ARPMODULE_H
#define HHUOS_ARPMODULE_H

#include "lib/util/stream/InputStream.h"
#include "network/NetworkModule.h"
#include "kernel/log/Logger.h"
#include "ArpHeader.h"

namespace Network::Arp {

class ArpModule : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    ArpModule() = default;

    /**
     * Copy Constructor.
     */
    ArpModule(const ArpModule &other) = delete;

    /**
     * Assignment operator.
     */
    ArpModule &operator=(const ArpModule &other) = delete;

    /**
     * Destructor.
     */
    ~ArpModule() = default;

    void readPacket(Util::Stream::InputStream &stream) override;

private:

    static void discardPacket(Util::Stream::InputStream &stream, const ArpHeader &arpHeader);

    static Kernel::Logger log;
};

}

#endif

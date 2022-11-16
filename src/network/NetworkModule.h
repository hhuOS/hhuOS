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

#ifndef HHUOS_NETWORKMODULE_H
#define HHUOS_NETWORKMODULE_H

#include "lib/util/stream/InputStream.h"
#include "lib/util/data/HashMap.h"

namespace Network {

class NetworkModule {

public:
    /**
     * Default Constructor.
     */
    NetworkModule() = default;

    /**
     * Copy Constructor.
     */
    NetworkModule(const NetworkModule &other) = delete;

    /**
     * Assignment operator.
     */
    NetworkModule &operator=(const NetworkModule &other) = delete;

    /**
     * Destructor.
     */
    ~NetworkModule() = default;

    virtual void readPacket(Util::Stream::InputStream &stream) = 0;

    void registerNextLayerModule(uint32_t protocolId, NetworkModule *module);

    bool isNextLayerTypeSupported(uint32_t protocolId);

protected:

    void invokeNextLayerModule(uint32_t protocolId, Util::Stream::InputStream &stream);

private:

    Util::Data::HashMap<uint32_t, NetworkModule*> nextLayerModules;
};

}

#endif

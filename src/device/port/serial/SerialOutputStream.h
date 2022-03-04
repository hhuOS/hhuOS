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

#ifndef HHUOS_SERIALOUTPUTSTREAM_H
#define HHUOS_SERIALOUTPUTSTREAM_H

#include "lib/util/stream/OutputStream.h"
#include "SerialPort.h"

namespace Device {

class SerialOutputStream : public Util::Stream::OutputStream {

public:
    /**
     * Default Constructor.
     */
    explicit SerialOutputStream(SerialPort *port);

    /**
     * Copy constructor.
     */
    SerialOutputStream(const SerialOutputStream &other) = delete;

    /**
     * Assignment operator.
     */
    SerialOutputStream &operator=(const SerialOutputStream &other) = delete;

    /**
     * Destructor.
     */
    ~SerialOutputStream() override;

    /**
     * Overriding function from OutputStream
     */
    void write(uint8_t c) override;

    /**
     * Overriding function from OutputStream
     */
    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

private:

    SerialPort *port;

};

}

#endif
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

#ifndef HHUOS_MACADDRESS_H
#define HHUOS_MACADDRESS_H

#include "NetworkAddress.h"

namespace Network {

class MacAddress : public NetworkAddress {

public:

    static const constexpr uint8_t ADDRESS_LENGTH = 6;

    /**
     * Default Constructor.
     */
    MacAddress() = default;

    /**
     * Constructor.
     */
    explicit MacAddress(uint8_t *buffer);

    /**
     * Copy Constructor.
     */
    MacAddress(const MacAddress &other);

    /**
     * Assignment operator.
     */
    MacAddress &operator=(const MacAddress &other);

    /**
     * Destructor.
     */
    ~MacAddress() override = default;

    static MacAddress createBroadcastAddress();

    [[nodiscard]] NetworkAddress* createCopy() const override;

    void read(Util::Stream::InputStream &stream) override;

    void write(Util::Stream::OutputStream &stream) const override;

    void setAddress(const Util::Memory::String &string) override;

    void setAddress(const uint8_t *buffer) override;

    void getAddress(uint8_t *buffer) const override;

    [[nodiscard]] uint8_t getLength() const override;

    [[nodiscard]] Util::Memory::String toString() const override;

    [[nodiscard]] Type getType() const override;

    [[nodiscard]] bool isBroadcastAddress() const;
    
private:

    uint8_t *buffer = new uint8_t[ADDRESS_LENGTH];

};

}

#endif

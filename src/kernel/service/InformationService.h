/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_INFORMATIONSERVICE_H
#define HHUOS_INFORMATIONSERVICE_H

#include <cstdint>

#include "Service.h"

namespace Device {
class Acpi;
class SmBios;
}  // namespace Device

namespace Kernel {
class Multiboot;

class InformationService : public Service {

public:
    /**
     * Constructor.
     */
    explicit InformationService(const Kernel::Multiboot *multiboot);

    /**
     * Copy Constructor.
     */
    InformationService(const InformationService &copy) = delete;

    /**
     * Assignment operator.
     */
    InformationService &operator=(const InformationService &other) = delete;

    /**
     * Destructor.
     */
    ~InformationService() override = default;

    void setAcpi(const Device::Acpi *acpi);

    void setSmBios(const Device::SmBios *smBios);

    [[nodiscard]] const Multiboot& getMultibootInformation() const;

    [[nodiscard]] const Device::Acpi& getAcpi() const;

    [[nodiscard]] const Device::SmBios& getSmBios() const;

    static const constexpr uint8_t SERVICE_ID = 9;

private:

    const Multiboot *multiboot;
    const Device::Acpi *acpi = nullptr;
    const Device::SmBios *smBios = nullptr;
};

}

#endif

/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "Service.h"
#include "lib/util/io/file/ElfFile.h"

namespace Util {
namespace Io {
}  // namespace Io
}  // namespace Util

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

    [[nodiscard]] const char* getSymbolName(uint32_t symbolAddress);

    static const constexpr uint8_t SERVICE_ID = 9;

private:

    static void* mapElfSection(const Util::Io::ElfFile::SectionHeader &sectionHeader);

    const Multiboot *multiboot;
    const Device::Acpi *acpi = nullptr;
    const Device::SmBios *smBios = nullptr;

    uint32_t symbolTableSize = 0;
    const Util::Io::ElfFile::SymbolEntry *symbolTable = nullptr;
    const char *stringTable = nullptr;
};

}

#endif

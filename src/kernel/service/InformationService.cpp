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

#include "InformationService.h"

Kernel::InformationService::InformationService(const Kernel::Multiboot *multiboot) : multiboot(multiboot) {}

void Kernel::InformationService::setAcpi(const Device::Acpi *acpi) {
    InformationService::acpi = acpi;
}

void Kernel::InformationService::setSmBios(const Device::SmBios *smBios) {
    InformationService::smBios = smBios;
}

const Kernel::Multiboot& Kernel::InformationService::getMultibootInformation() const {
    return *multiboot;
}

const Device::Acpi& Kernel::InformationService::getAcpi() const {
    return *acpi;
}

const Device::SmBios& Kernel::InformationService::getSmBios() const {
    return *smBios;
}

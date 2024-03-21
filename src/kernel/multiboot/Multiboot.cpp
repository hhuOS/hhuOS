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

#include "kernel/memory/Paging.h"

#include "kernel/memory/MemoryLayout.h"
#include "Multiboot.h"
#include "kernel/multiboot/Multiboot.h"

#include "kernel/service/MemoryService.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/Address.h"
#include "lib/util/collection/ArrayList.h"

namespace Kernel {

Util::String Multiboot::getBootloaderName() const {
    if (!hasTag(BOOT_LOADER_NAME)) {
        return "";
    }

    return getTag<BootLoaderName>(BOOT_LOADER_NAME).string;
}

bool Multiboot::hasKernelOption(const Util::String &key) const {
    if (!hasTag(BOOT_COMMAND_LINE)) {
        return false;
    }

    auto commandLine = getTag<BootCommandLine>(BOOT_COMMAND_LINE).string;
    Util::Array<Util::String> options = Util::String(commandLine).split(" ");

    for (const Util::String &option : options) {
        if (option.split("=")[0] == key) {
            return true;
        }
    }

    return false;
}

Util::String Multiboot::getKernelOption(const Util::String &key) const {
    if (!hasTag(BOOT_COMMAND_LINE)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Kernel options are not available!");
    }

    auto commandLine = getTag<BootCommandLine>(BOOT_COMMAND_LINE).string;
    Util::Array<Util::String> options = Util::String(commandLine).split(" ");

    for (const Util::String &option : options) {
        auto split = option.split("=");
        if (split[0] == key) {
            return split[1];
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested kernel option is not available!");
}

Util::Array<Util::String> Multiboot::getModuleNames() const {
    auto list = Util::ArrayList<Util::String>();
    auto currentAddress = reinterpret_cast<uint32_t>(this) + sizeof(Multiboot);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *module = reinterpret_cast<const Module*>(currentTag);
            list.add(module->name);
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return list.toArray();
}

bool Multiboot::isModuleLoaded(const Util::String &moduleName) const {
    auto currentAddress = reinterpret_cast<uint32_t>(this) + sizeof(Multiboot);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *module = reinterpret_cast<const Module*>(currentTag);
            if (moduleName == module->name) {
                return true;
            }
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return false;
}

const Multiboot::Module& Multiboot::getModule(const Util::String &moduleName) const {
    auto currentAddress = reinterpret_cast<uint32_t>(this) + sizeof(Multiboot);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == MODULE) {
            auto *module = reinterpret_cast<Module*>(const_cast<TagHeader*>(currentTag));
            if (moduleName == module->name) {
                return *module;
            }
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Multiboot: Requested module is not loaded!");
}

bool Multiboot::hasTag(Multiboot::TagType type) const {
    auto currentAddress = reinterpret_cast<uint32_t>(this) + sizeof(Multiboot);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        if (currentTag->type == type) {
            return true;
        }

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return false;
}

Util::Array<Multiboot::TagType> Multiboot::getAvailableTagTypes() const {
    Util::ArrayList<TagType> types;
    auto currentAddress = reinterpret_cast<uint32_t>(this) + sizeof(Multiboot);
    auto *currentTag = reinterpret_cast<const TagHeader*>(currentAddress);

    while (currentTag->type != TERMINATE) {
        types.add(currentTag->type);

        currentAddress += currentTag->size;
        currentAddress = currentAddress % 8 == 0 ? currentAddress : (currentAddress / 8) * 8 + 8;
        currentTag = reinterpret_cast<const TagHeader*>(currentAddress);
    }

    return types.toArray();
}

uint32_t Multiboot::getSize() const {
    return size;
}

}

/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ElfLoader.h"

KernelSymbols *ElfLoader::linkerService = nullptr;

extern "C" {
    int resolveSymbol(Elf &elf, uint32_t index);
}

int resolveSymbol(Elf &elf, uint32_t index) {
    return elf.resolveSymbol(index);
}

bool ElfLoader::load(File *file, int argc, char *argv[]) {

    Elf &elf = Elf::from(file, 0x800000);

    if (!elf.isValid()) {
        return false;
    }

    void (*elfProgram)(int, char*[]) = (void(*)(int, char*[])) elf.getEntryPoint();

    elfProgram(argc, argv);

    return true;
}

uint32_t ElfLoader::getSymbol(const String &name) {

    return KernelSymbols::get(name);
}

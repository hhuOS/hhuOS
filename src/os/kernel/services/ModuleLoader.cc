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

#include "lib/elf/ElfConstants.h"
#include "ModuleLoader.h"

using namespace ElfConstants;

ModuleLoader::Status ModuleLoader::load(File *file) {

    Module *module = new Module();

    uint32_t fileSize = (uint32_t) file->getLength();

    module->buffer = new char[fileSize];

    module->base = (uint32_t) module->buffer;

    file->readBytes(module->buffer, fileSize);

    module->fileHeader = (FileHeader *) module->buffer;

    if ( !module->isValid() ) {
        return Status::INVALID;
    }

    module->loadSectionNames();

    module->loadSections();

    module->parseSymbolTable();

    module->relocate();

    module->init = (int(*)()) module->getSymbol("module_init");

    module->fini = (int(*)()) module->getSymbol("module_fini");

    if (module->initialize() != 0) {
        module->finalize();
        return Status::ERROR;
    };

    modules.put(file->getName(), module);

    return Status::OK;
}

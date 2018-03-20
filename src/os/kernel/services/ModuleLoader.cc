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

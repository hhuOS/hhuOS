#include "Elf.h"
#include "ElfLoader.h"

extern "C" {
    void resolveSymbolHelper();
}

Elf::Elf(File *elfFile, uint32_t base) : base(base) , elfFile(elfFile), sectionHeaders(47), symbolTable(47) {

}

Elf &Elf::from(File *elfFile, uint32_t base) {

    Elf *elf = new Elf(elfFile, base);

    elf->readFileHeader();

    if ( !elf->fileHeader.isValid() ) {
        elf->valid = false;
        return *elf;
    }

    if ( !elf->fileHeader.hasProgramEntries() ) {
        elf->valid = false;
        return *elf;
    }

    elf->readSectionNames();

    elf->readProgramHeaders();

    elf->readSectionHeaders();

    elf->parseDynamicTable();

    elf->loadDynamicSymbols();

    elf->relocate();

    elf->adjustGlobalOffsetTable();

    return *elf;
}

void Elf::readFileHeader() {
    elfFile->readBytes((char*) &fileHeader, sizeof(FileHeader));
}

void Elf::readProgramHeaders() {
    ProgramHeader *programHeader = new ProgramHeader;

    for (elf32_word i = 0; i < fileHeader.programHeaderEntries; i++) {
        elfFile->setPos(fileHeader.programHeader + i * fileHeader.programHeaderEntrySize, File::START);
        elfFile->readBytes((char*) programHeader, sizeof(ProgramHeader));

        processProgramHeader(*programHeader);
    }

    delete programHeader;
}

void Elf::processProgramHeader(const ProgramHeader &programHeader) {

    switch (programHeader.type) {
        case ProgramHeaderType::LOAD:
            elfFile->setPos(programHeader.offset, File::START);
            elfFile->readBytes((char*) base + programHeader.virtualAddress, programHeader.fileSize);

            memset((void*) (base + programHeader.virtualAddress + programHeader.fileSize), 0, programHeader.memorySize - programHeader.fileSize);
            break;
        case ProgramHeaderType::DYNAMIC:
            dynamic = (DynamicEntry*) base + programHeader.virtualAddress;
            break;
        default:
            break;
    }

}

void Elf::parseDynamicTable() {

    if (dynamic == nullptr) {
        return;
    }

    DynamicEntry *entry = dynamic;

    while (entry->tag != DynamicTag::NONE) {

        switch (entry->tag) {
            case DynamicTag::STRTAB :
                dynamicStringTable = (char*) (base + entry->value);
                break;
            case DynamicTag::STRSZ :
                dynamicStringTableSize = entry->value;
                break;
            case DynamicTag::SYMTAB :
                dynamicSymbolTable = (SymbolEntry*) (base + entry->value);
                break;
            case DynamicTag::PLTGOT :
                globalOffsetTable = (uint32_t*) (base + entry->value);
                break;
            case DynamicTag::PLTRELSZ :
                relocationTableEntries = entry->value / sizeof(RelocationEntry);
                break;
            case DynamicTag::HASH :
                hashTable = (uint32_t*) (base + entry->value);
                dynamicSymbolTableEntries = hashTable[1] / sizeof(SymbolEntry);
                break;
            default:
                break;
        }

        entry++;
    }

}

void Elf::loadDynamicSymbols() {

    if (dynamicSymbolTable == nullptr) {
        return;
    }

    SymbolEntry *entry = nullptr;
    for (uint32_t i = 1; i < dynamicSymbolTableEntries; i++) {

        entry = &dynamicSymbolTable[i];

        if (entry->section != 0) {
            symbolTable.put(&dynamicStringTable[entry->nameOffset], entry->value);
        }

    }

}

void Elf::relocate() {

//    SectionHeader sectionHeader = sectionHeaders.get(SECTION_NAME_REL).value();
//
//    RelocationEntry *relocationTable = (RelocationEntry*) base + sectionHeader.virtualAddress;
//
//    uint8_t index;
//    RelocationType type;
//    SymbolEntry symbol;
//    char* symbolName;
//    for (uint32_t i = 0; i < relocationTableEntries; i++) {
//
//        type = relocationTable[i].getType();
//        index = relocationTable[i].getIndex();
//        symbol = dynamicSymbolTable[index];
//        symbolName = &dynamicStringTable[symbol.nameOffset];
//
//        switch (type) {
//            case RelocationType::R_386_JMP_SLOT :
//                break;
//            default:
//                break;
//        }
//    }

}

void Elf::readSectionNames() {
    SectionHeader *sectionHeader = new SectionHeader;
    elfFile->setPos(fileHeader.sectionHeader + fileHeader.sectionHeaderStringIndex * fileHeader.sectionHeaderEntrySize, File::START);
    elfFile->readBytes((char*) sectionHeader, sizeof(SectionHeader));

    sectionNames = new char[sectionHeader->size];
    elfFile->setPos(sectionHeader->offset, File::START);
    elfFile->readBytes(sectionNames, sectionHeader->size);

    delete sectionHeader;
}

void Elf::readSectionHeaders() {
    SectionHeader *sectionHeader = new SectionHeader;

    for (elf32_word i = 0; i < fileHeader.sectionHeaderEntries; i++) {
        elfFile->setPos(fileHeader.sectionHeader + i * fileHeader.sectionHeaderEntrySize, File::START);
        elfFile->readBytes((char*) sectionHeader, sizeof(SectionHeader));

        if (sectionHeader->type == SectionHeaderType::NONE) {
            continue;
        }

        sectionHeaders.put(&sectionNames[sectionHeader->nameOffset], *sectionHeader);
    }

    delete sectionHeader;
}

bool Elf::isValid() {
    return valid;
}

uint32_t Elf::getEntryPoint() {
    return fileHeader.entry;
}

void Elf::adjustGlobalOffsetTable() {
    SectionHeader sectionHeader = sectionHeaders.get(SECTION_NAME_GOT).value();

    char section[sectionHeader.size];
    elfFile->setPos(sectionHeader.offset, File::START);
    elfFile->readBytes((char*) section, sectionHeader.size);

    memcpy((void*) sectionHeader.virtualAddress, section, sectionHeader.size);

    ((uint32_t*) sectionHeader.virtualAddress)[1] = (uint32_t) this;
    ((uint32_t*) sectionHeader.virtualAddress)[2] = (uint32_t) &resolveSymbolHelper;
}

SectionHeader Elf::getSectionHeader(const String &name) {
    Optional<SectionHeader> optional = sectionHeaders.get(name);

    if (optional.isNull()) {
        return {};
    }

    return optional.value();
}

uint32_t Elf::resolveSymbol(uint32_t index) {

    SectionHeader sectionHeader = getSectionHeader(SECTION_NAME_REL);

    RelocationEntry *relocationTable = (RelocationEntry*) base + sectionHeader.virtualAddress;
    RelocationEntry *relocationEntry = &relocationTable[index / sizeof(RelocationEntry)];
    SymbolEntry *symbol = &dynamicSymbolTable[relocationEntry->getIndex()];
    char *symbolName = &dynamicStringTable[symbol->nameOffset];

    uint32_t *symbolAddress = nullptr;

    Optional<uint32_t> optional = symbolTable.get(symbolName);

    if (!optional.isNull()) {
        symbolAddress = (uint32_t*) base + optional.value();
    } else {
        symbolAddress = (uint32_t*) ElfLoader::getSymbol(symbolName);
    }

    uint32_t *offset = (uint32_t*) base + relocationEntry->offset;
    *offset = (uint32_t) symbolAddress;
    return *offset;
}



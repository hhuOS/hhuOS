#include <lib/memory/MemoryUtil.h>
#include <kernel/memory/Paging.h>
#include "Elf.h"

Elf::Elf(File *file) : file(file) {

}

Elf::~Elf() {
    delete file;
    delete sectionNames;
    delete programHeaders;
}

Elf *Elf::load(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        return nullptr;
    }

    auto elf = new Elf(file);

    elf->readFileHeader();

    if(!elf->isValid()) {
        delete elf;
        return nullptr;
    }

    elf->readSectionNames();
    elf->readSectionHeaders();
    elf->readProgramHeaders();

    return elf;
}

void Elf::readFileHeader() {
    file->readBytes(reinterpret_cast<char*>(&fileHeader), sizeof(ElfConstants::FileHeader));
}

bool Elf::isValid() {
    return fileHeader.isValid();
}

void Elf::readSectionNames() {
    ElfConstants::SectionHeader header{};

    file->setPos(fileHeader.sectionHeader + fileHeader.sectionHeaderStringIndex * fileHeader.sectionHeaderEntrySize, File::START);

    file->readBytes(reinterpret_cast<char *>(&header), sizeof(ElfConstants::SectionHeader));

    sectionNames = new char[header.size];

    file->setPos(header.offset, File::START);

    file->readBytes(sectionNames, header.size);
}

void Elf::readProgramHeaders() {
    programHeaders = new ElfConstants::ProgramHeader[fileHeader.programHeaderEntries];

    ElfConstants::ProgramHeader programHeader{};

    for (elf32_word i = 0; i < fileHeader.programHeaderEntries; i++) {

        file->setPos(fileHeader.programHeader + i * fileHeader.programHeaderEntrySize, File::START);

        file->readBytes(reinterpret_cast<char *>(&programHeader), sizeof(ElfConstants::ProgramHeader));

        programHeaders[i] = programHeader;
    }
}

void Elf::readSectionHeaders() {
    ElfConstants::SectionHeader header{};

    for (elf32_word i = 0; i < fileHeader.sectionHeaderEntries; i++) {

        file->setPos(fileHeader.sectionHeader + i * fileHeader.sectionHeaderEntrySize, File::START);

        file->readBytes(reinterpret_cast<char *>(&header), sizeof(ElfConstants::SectionHeader));

        if (header.type == ElfConstants::SectionHeaderType::NONE) {
            continue;
        }

        sectionHeaders.put(&sectionNames[header.nameOffset], header);
    }
}

uint32_t Elf::getSizeInMemory() {
    uint32_t ret = 0;

    for(int i = 0; i < fileHeader.programHeaderEntries; i++) {
        auto header = programHeaders[i];

        if(header.type == ElfConstants::ProgramHeaderType::LOAD) {
            const auto size = header.virtualAddress + header.memorySize;

            if(size > ret) {
                ret = size;
            }
        }
    }

    return MemoryUtil::alignUp(ret, PAGESIZE);
}

void Elf::loadProgram() {
    for(int i = 0; i < fileHeader.programHeaderEntries; i++) {
        auto header = programHeaders[i];

        if(header.type == ElfConstants::ProgramHeaderType::LOAD) {
            file->setPos(header.offset);
            file->readBytes(reinterpret_cast<char *>(header.virtualAddress), header.fileSize);
        }
    }
}


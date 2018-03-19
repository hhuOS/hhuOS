#include "ElfConstants.h"

using namespace elfconstants;

bool FileHeader::isValid() {
    if (magic[0] != 0x7F ||
        magic[1] != 'E'  ||
        magic[2] != 'L'  ||
        magic[3] != 'F') {
        return false;
    }

    if (architecture != Architecture::BIT_32||
        byteOrder != ByteOrder::LITTLE_ENDIAN     ||
        machine != MachineType::X86) {
        return false;
    }

    return !(type != ElfType::RELOCATABLE && type != ElfType::EXECUTABLE);
}

bool FileHeader::hasProgramEntries() {
    return programHeaderEntries != 0;
}

uint8_t RelocationEntry::getIndex() {
    return (uint8_t) (info >> 8);
}

RelocationType RelocationEntry::getType() {
    return RelocationType(info & 0xFF);
}

SymbolBinding SymbolEntry::getBinding() {
    return SymbolBinding(info >> 4);
}

SymbolType SymbolEntry::getType() {
    return SymbolType(info & 0xF);
}

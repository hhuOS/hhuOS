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

#include "System.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/BufferedOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/elf/File.h"
#include "Constants.h"
#include "Address.h"

namespace Util {

Io::FileInputStream System::inStream(Util::Io::STANDARD_INPUT);
Io::BufferedInputStream System::bufferedInStream(inStream);
Io::InputStream &System::in = System::bufferedInStream;
Io::FileOutputStream System::outStream(Util::Io::STANDARD_OUTPUT);
Io::BufferedOutputStream System::bufferedOutStream(outStream);
Io::PrintStream System::out(bufferedOutStream);
Io::FileOutputStream System::errorStream(Util::Io::STANDARD_ERROR);
Io::BufferedOutputStream System::bufferedErrorStream(errorStream);
Io::PrintStream System::error(bufferedErrorStream);

bool System::call(System::Code code, uint32_t paramCount...) {
    va_list args;
    va_start(args, paramCount);
    bool result = false;

    call(code, result, paramCount, args);

    va_end(args);
    return result;
}

void System::call(Code code, bool &result, uint32_t paramCount, va_list args) {
    auto ebxValue = static_cast<uint32_t>(code | (paramCount << 8));
    auto ecxValue = reinterpret_cast<uint32_t>(args);
    auto edxValue = reinterpret_cast<uint32_t>(&result);

    asm volatile (
            "push %%ebx;"
            "push %%ecx;"
            "push %%edx;"

            "mov %0, %%ebx;"
            "mov %1, %%ecx;"
            "mov %2, %%edx;"
            "int $0x86;"

            "pop %%edx;"
            "pop %%ecx;"
            "pop %%ebx;"
            : :
            "r"(ebxValue),
            "r"(ecxValue),
            "r"(edxValue)
            : "eax", "ebx", "ecx"
            );
}

void System::printStackTrace(Io::PrintStream &stream, uint32_t minEbp) {
    uint32_t *ebp = nullptr;
    asm volatile (
            "mov %%ebp, (%0);"
            : :
            "r"(&ebp)
            :
            "eax"
            );


    // If we handle an exception in user space, we skip the kernel space exception handler
    while (reinterpret_cast<uint32_t>(ebp) < minEbp) {
        ebp = reinterpret_cast<uint32_t*>(ebp[0]);
    }

    while (reinterpret_cast<uint32_t>(ebp) >= minEbp) {
        auto eip = ebp[1];
        stream << Util::String::format("0x%08x", eip) << Util::Io::PrintStream::flush;

        auto *symbolName = getSymbolName(eip);
        while (symbolName == nullptr && eip >= Util::USER_SPACE_MEMORY_START_ADDRESS) {
            symbolName = getSymbolName(--eip);
        }

        stream << " " << symbolName << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

        if (!Util::Address<uint32_t>(symbolName).compareString("main")) {
            break;
        }

        ebp = reinterpret_cast<uint32_t*>(ebp[0]);
    }
}

const char* System::getSymbolName(uint32_t symbolAddress) {
    auto &addressSpaceHeader = Util::System::getAddressSpaceHeader();

    for (uint32_t i = 0; i < addressSpaceHeader.symbolTableSize / sizeof(Util::Io::Elf::SymbolEntry); i++) {
        const auto &symbol = *(addressSpaceHeader.symbolTable + i);
        if (symbol.value == symbolAddress && symbol.getSymbolType() == Util::Io::Elf::SymbolType::FUNC) {
            if (symbol.getSymbolType() == Util::Io::Elf::SymbolType::FUNC) {
                return addressSpaceHeader.stringTable + symbol.nameOffset;
            }
        }
    }

    return nullptr;
}

System::AddressSpaceHeader &System::getAddressSpaceHeader() {
    return *reinterpret_cast<AddressSpaceHeader*>(USER_SPACE_MEMORY_START_ADDRESS);
}

}
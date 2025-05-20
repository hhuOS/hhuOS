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

#include "System.h"

#include "base/Address.h"
#include "base/Constants.h"
#include "base/String.h"
#include "io/file/File.h"
#include "io/file/elf/File.h"
#include "io/stream/BufferedInputStream.h"
#include "io/stream/BufferedOutputStream.h"
#include "io/stream/FileInputStream.h"
#include "io/stream/FileOutputStream.h"

namespace Util {

Io::FileInputStream inStream(Io::STANDARD_INPUT);
Io::BufferedInputStream bufferedInStream(inStream);
Io::InputStream &System::in = bufferedInStream;

Io::FileOutputStream outStream(Io::STANDARD_OUTPUT);
Io::BufferedOutputStream bufferedOutStream(outStream);
Io::PrintStream System::out(bufferedOutStream);

Io::FileOutputStream errorStream(Io::STANDARD_ERROR);
Io::BufferedOutputStream bufferedErrorStream(errorStream);
Io::PrintStream System::error(bufferedErrorStream);

void systemCall(System::Code code, bool &result, size_t paramCount, va_list args) {
    asm volatile (
            "int $0x86;"
            : "=m"(result)
            : "b"(code | (paramCount << 8)), "c"(args), "d"(&result)
            );
}

const char* getSymbolName(const size_t symbolAddress) {
    const auto &addressSpaceHeader = System::getAddressSpaceHeader();

    for (size_t i = 0; i < addressSpaceHeader.symbolTableSize / sizeof(Io::Elf::SymbolEntry); i++) {
        const auto &symbol = *(addressSpaceHeader.symbolTable + i);

        if (symbol.value == symbolAddress && symbol.getSymbolType() == Io::Elf::SymbolType::FUNC) {
            if (symbol.getSymbolType() == Io::Elf::SymbolType::FUNC) {
                return addressSpaceHeader.stringTable + symbol.nameOffset;
            }
        }
    }

    return nullptr;
}

bool System::call(const Code code, const size_t paramCount...) {
    va_list args;
    va_start(args, paramCount);
    bool result = false;

    systemCall(code, result, paramCount, args);

    va_end(args);
    return result;
}

void System::printStackTrace(Io::PrintStream &stream, size_t minEbp) {
    size_t *ebp = nullptr;
    asm volatile (
            "mov %%ebp, %0;"
            : "=m"(ebp)
            );

    // If we handle an exception in user space, we skip the kernel space exception handler
    while (reinterpret_cast<size_t>(ebp) < minEbp) {
        ebp = reinterpret_cast<size_t*>(ebp[0]);
    }

    while (reinterpret_cast<size_t>(ebp) >= minEbp) {
        auto eip = ebp[1];
        stream << String::format("0x%08x", eip) << Io::PrintStream::flush;

        auto *symbolName = getSymbolName(eip);
        while (symbolName == nullptr && eip >= USER_SPACE_MEMORY_START_ADDRESS) {
            symbolName = getSymbolName(--eip);
        }

        stream << " " << symbolName << Io::PrintStream::endl << Io::PrintStream::flush;

        if (!Address(symbolName).compareString(Address("main"))) {
            break;
        }

        ebp = reinterpret_cast<size_t*>(ebp[0]);
    }
}

System::AddressSpaceHeader& System::getAddressSpaceHeader() {
    return *reinterpret_cast<AddressSpaceHeader*>(USER_SPACE_MEMORY_START_ADDRESS);
}

}
/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/stream/BufferedInputStream.h"
#include "lib/util/stream/BufferedOutputStream.h"
#include "lib/util/stream/FileInputStream.h"
#include "lib/util/stream/FileOutputStream.h"
#include "lib/util/stream/InputStreamReader.h"
#include "lib/util/stream/PrintWriter.h"

namespace Util {

Stream::FileInputStream System::inStream(0);
Stream::BufferedInputStream System::bufferedInStream(inStream);
Stream::InputStreamReader System::in(bufferedInStream);
Stream::FileOutputStream System::outStream(1);
Stream::BufferedOutputStream System::bufferedOutStream(outStream);
Stream::PrintWriter System::out(bufferedOutStream);
Stream::FileOutputStream System::errorStream(2);
Stream::BufferedOutputStream System::bufferedErrorStream(errorStream);
Stream::PrintWriter System::error(bufferedErrorStream);
const char *System::errorMessage = "";

System::Result System::call(System::Code code, uint32_t paramCount...) {
    va_list args;
    va_start(args, paramCount);
    Result result;

    call(code, result, paramCount, args);

    va_end(args);
    return result;
}

void System::call(Code code, Result &result, uint32_t paramCount, va_list args) {
    auto eaxValue = static_cast<uint32_t>(code | (paramCount << 16u));
    auto ebxValue = reinterpret_cast<uint32_t>(args);
    auto ecxValue = reinterpret_cast<uint32_t>(&result);

    asm volatile (
            "push %%eax;"
            "push %%ebx;"
            "push %%ecx;"

            "mov %0, %%eax;"
            "mov %1, %%ebx;"
            "mov %2, %%ecx;"
            "int $0x86;"

            "pop %%ecx;"
            "pop %%ebx;"
            "pop %%eax;"
            : :
            "r"(eaxValue),
            "r"(ebxValue),
            "r"(ecxValue));
}

}
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

#include <cstdint>
#include "lib/util/system/System.h"
#include "lib/util/memory/Address.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/memory/MmxAddress.h"

static const constexpr uint32_t BUFFER_SIZE = 1024 * 1024;

uint32_t benchmarkMemset(Util::Memory::Address<uint32_t> &target, uint32_t iterations) {
    auto start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.setRange(i, BUFFER_SIZE);
    }

    return Util::Time::getSystemTime().toMilliseconds() - start;
}

uint32_t benchmarkMemcpy(Util::Memory::Address<uint32_t> &source, Util::Memory::Address<uint32_t> &target, uint32_t iterations) {
    auto start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.copyRange(source, BUFFER_SIZE);
    }

    return Util::Time::getSystemTime().toMilliseconds() - start;
}

int32_t main(int32_t argc, char *argv[]) {
    if (!Util::Memory::MmxAddress<uint32_t>::isAvailable()) {
        Util::System::out << "MMX is not supported by this CPU!" << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl;
    }

    auto iterations = static_cast<uint32_t>(argc > 1 ? Util::Memory::String::parseInt(argv[1]) : 100);
    auto *buffer1 = new uint8_t[BUFFER_SIZE];
    auto *buffer2 = new uint8_t[BUFFER_SIZE];
    auto source = Util::Memory::Address<uint32_t>(buffer1);
    auto target = Util::Memory::Address<uint32_t>(buffer2);
    auto mmxTarget = Util::Memory::MmxAddress<uint32_t>(buffer2);

    Util::System::out << "Ensuring buffers are mapped in..." << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl;
    source.setRange(0, BUFFER_SIZE);
    target.setRange(0, BUFFER_SIZE);

    Util::System::out << "Running memset benchmark without MMX..." << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl;
    auto start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.setRange(i, BUFFER_SIZE);
    }
    auto memsetResult = Util::Time::getSystemTime().toMilliseconds() - start;

    Util::System::out << "Running memcpy benchmark without MMX..." << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl;
    start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.copyRange(source, BUFFER_SIZE);
    }
    auto memcpyResult = Util::Time::getSystemTime().toMilliseconds() - start;

    Util::System::out << "Running memset benchmark with MMX..." << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl;
    start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        mmxTarget.setRange(i, BUFFER_SIZE);
    }
    Util::Memory::MmxAddress<uint32_t>::endMmxOperation();
    auto memsetMmxResult = Util::Time::getSystemTime().toMilliseconds() - start;

    Util::System::out << "Running memcpy benchmark with MMX..." << Util::Stream::PrintWriter::flush << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl;
    start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        mmxTarget.copyRange(source, BUFFER_SIZE);
    }
    Util::Memory::MmxAddress<uint32_t>::endMmxOperation();
    auto memcpyMmxResult = Util::Time::getSystemTime().toMilliseconds() - start;

    delete[] buffer1;
    delete[] buffer2;

    double memsetSpeedup = (double) memsetResult / memsetMmxResult;
    double memcpySpeedup = (double) memcpyResult / memcpyMmxResult;

    auto memsetString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memsetSpeedup), static_cast<uint32_t>((memsetSpeedup - static_cast<uint32_t>(memsetSpeedup)) * 100));
    auto memcpyString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memcpySpeedup), static_cast<uint32_t>((memcpySpeedup - static_cast<uint32_t>(memcpySpeedup)) * 100));

    Util::System::out << "MMX disabled:" << Util::Stream::PrintWriter::endl
            << "memset: " << memsetResult << "ms" << Util::Stream::PrintWriter::endl
            << "memcpy: " << memcpyResult << "ms" << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::endl
            << "MMX enabled:" << Util::Stream::PrintWriter::endl
            << "memset: " << memsetMmxResult << "ms (" << memsetString << ")" << Util::Stream::PrintWriter::endl
            << "memcpy: " << memcpyMmxResult << "ms (" << memcpyString << ")" << Util::Stream::PrintWriter::endl <<  Util::Stream::PrintWriter::flush;

    return 0;
}
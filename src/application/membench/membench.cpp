/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/System.h"
#include "lib/util/base/Address.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/base/MmxAddress.h"
#include "lib/util/hardware/CpuId.h"
#include "lib/util/base/SseAddress.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/math/Math.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"

static const constexpr uint32_t BUFFER_SIZE = 1024 * 1024;

void benchmark(uint32_t iterations, const Util::Address<uint32_t> &source, const Util::Address<uint32_t> &target, uint32_t &memsetResult, uint32_t &memcpyResult) {
    auto start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.setRange(i, BUFFER_SIZE);
    }
    memsetResult = Util::Time::getSystemTime().toMilliseconds() - start;

    start = Util::Time::getSystemTime().toMilliseconds();
    for (uint32_t i = 0; i < iterations; i++) {
        target.copyRange(source, BUFFER_SIZE);
    }
    memcpyResult = Util::Time::getSystemTime().toMilliseconds() - start;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Memory bandwidth benchmark comparing different acceleration techniques.\n"
                               "Each iteration operates on 1 MiB of memory (Default: 100 iterations).\n"
                               "Usage: membench [ITERATIONS]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    auto iterations = static_cast<uint32_t>(arguments.length() == 0 ? 100 : Util::String::parseInt(arguments[0]));
    auto *buffer1 = new uint8_t[BUFFER_SIZE];
    auto *buffer2 = new uint8_t[BUFFER_SIZE];
    Util::Io::ByteArrayOutputStream resultStream;
    Util::Io::PrintStream resultWriter(resultStream);
    
    Util::System::out << "Ensuring buffers are mapped in..." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::Address<uint32_t>(buffer1).setRange(0, BUFFER_SIZE);
    Util::Address<uint32_t>(buffer2).setRange(0, BUFFER_SIZE);

    uint32_t memsetResult, memcpyResult;
    Util::System::out << "Running memory benchmarks without extensions..." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    benchmark(iterations, Util::Address<uint32_t>(buffer1), Util::Address<uint32_t>(buffer2), memsetResult, memcpyResult);
    resultWriter << "Without extensions:" << Util::Io::PrintStream::endl
                 << "memset: " << memsetResult << "ms" << Util::Io::PrintStream::endl
                 << "memcpy: " << memcpyResult << "ms" << Util::Io::PrintStream::endl;

    if (Util::Hardware::CpuId::getCpuFeatures().contains(Util::Hardware::CpuId::MMX)) {
        uint32_t memsetMmxResult, memcpyMmxResult;
        Util::System::out << "Running memory benchmarks with MMX..." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        benchmark(iterations, Util::MmxAddress<uint32_t>(buffer1), Util::MmxAddress<uint32_t>(buffer2), memsetMmxResult, memcpyMmxResult);

        Util::Math::endMmx();

        double memsetSpeedup = (double) memsetResult / memsetMmxResult;
        double memcpySpeedup = (double) memcpyResult / memcpyMmxResult;
        auto memsetString = Util::String::format("%u.%02ux", static_cast<uint32_t>(memsetSpeedup), static_cast<uint32_t>((memsetSpeedup - static_cast<uint32_t>(memsetSpeedup)) * 100));
        auto memcpyString = Util::String::format("%u.%02ux", static_cast<uint32_t>(memcpySpeedup), static_cast<uint32_t>((memcpySpeedup - static_cast<uint32_t>(memcpySpeedup)) * 100));
        
        resultWriter << Util::Io::PrintStream::endl << "MMX enabled:" << Util::Io::PrintStream::endl
                     << "memset: " << memsetMmxResult << "ms (" << memsetString << ")" << Util::Io::PrintStream::endl
                     << "memcpy: " << memcpyMmxResult << "ms (" << memcpyString << ")" << Util::Io::PrintStream::endl;
    }

    if (Util::Hardware::CpuId::getCpuFeatures().contains(Util::Hardware::CpuId::SSE)) {
        uint32_t memsetSseResult, memcpySseResult;
        Util::System::out << "Running memory benchmarks with SSE..." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        benchmark(iterations, Util::SseAddress<uint32_t>(buffer1), Util::SseAddress<uint32_t>(buffer2), memsetSseResult, memcpySseResult);

        double memsetSpeedup = (double) memsetResult / memsetSseResult;
        double memcpySpeedup = (double) memcpyResult / memcpySseResult;
        auto memsetString = Util::String::format("%u.%02ux", static_cast<uint32_t>(memsetSpeedup), static_cast<uint32_t>((memsetSpeedup - static_cast<uint32_t>(memsetSpeedup)) * 100));
        auto memcpyString = Util::String::format("%u.%02ux", static_cast<uint32_t>(memcpySpeedup), static_cast<uint32_t>((memcpySpeedup - static_cast<uint32_t>(memcpySpeedup)) * 100));

        resultWriter << Util::Io::PrintStream::endl << "SSE enabled:" << Util::Io::PrintStream::endl
                     << "memset: " << memsetSseResult << "ms (" << memsetString << ")" << Util::Io::PrintStream::endl
                     << "memcpy: " << memcpySseResult << "ms (" << memcpyString << ")" << Util::Io::PrintStream::endl;
    }
    
    delete[] buffer1;
    delete[] buffer2;

    Util::System::out << Util::Io::PrintStream::endl << resultStream.getContent() << Util::Io::PrintStream::flush;
    return 0;
}
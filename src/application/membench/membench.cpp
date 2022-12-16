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
#include "lib/util/cpu/CpuId.h"
#include "lib/util/memory/SseAddress.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/math/Math.h"
#include "lib/util/ArgumentParser.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/stream/PrintWriter.h"

static const constexpr uint32_t BUFFER_SIZE = 1024 * 1024;

void benchmark(uint32_t iterations, const Util::Memory::Address<uint32_t> &source, const Util::Memory::Address<uint32_t> &target, uint32_t &memsetResult, uint32_t &memcpyResult) {
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
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    auto iterations = static_cast<uint32_t>(arguments.length() == 0 ? 100 : Util::Memory::String::parseInt(arguments[0]));
    auto *buffer1 = new uint8_t[BUFFER_SIZE];
    auto *buffer2 = new uint8_t[BUFFER_SIZE];
    Util::Stream::ByteArrayOutputStream resultStream;
    Util::Stream::PrintWriter resultWriter(resultStream);
    
    Util::System::out << "Ensuring buffers are mapped in..." << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    Util::Memory::Address<uint32_t>(buffer1).setRange(0, BUFFER_SIZE);
    Util::Memory::Address<uint32_t>(buffer2).setRange(0, BUFFER_SIZE);

    uint32_t memsetResult, memcpyResult;
    Util::System::out << "Running memory benchmarks without extensions..." << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
    benchmark(iterations, Util::Memory::Address<uint32_t>(buffer1), Util::Memory::Address<uint32_t>(buffer2), memsetResult, memcpyResult);
    resultWriter << "Without extensions:" << Util::Stream::PrintWriter::endl
                 << "memset: " << memsetResult << "ms" << Util::Stream::PrintWriter::endl
                 << "memcpy: " << memcpyResult << "ms" << Util::Stream::PrintWriter::endl;

    if (Util::Cpu::CpuId::getCpuFeatures().contains(Util::Cpu::CpuId::MMX)) {
        uint32_t memsetMmxResult, memcpyMmxResult;
        Util::System::out << "Running memory benchmarks with MMX..." << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        benchmark(iterations, Util::Memory::MmxAddress<uint32_t>(buffer1), Util::Memory::MmxAddress<uint32_t>(buffer2), memsetMmxResult, memcpyMmxResult);

        Util::Math::Math::endMmx();

        double memsetSpeedup = (double) memsetResult / memsetMmxResult;
        double memcpySpeedup = (double) memcpyResult / memcpyMmxResult;
        auto memsetString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memsetSpeedup), static_cast<uint32_t>((memsetSpeedup - static_cast<uint32_t>(memsetSpeedup)) * 100));
        auto memcpyString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memcpySpeedup), static_cast<uint32_t>((memcpySpeedup - static_cast<uint32_t>(memcpySpeedup)) * 100));
        
        resultWriter << Util::Stream::PrintWriter::endl << "MMX enabled:" << Util::Stream::PrintWriter::endl
                     << "memset: " << memsetMmxResult << "ms (" << memsetString << ")" << Util::Stream::PrintWriter::endl
                     << "memcpy: " << memcpyMmxResult << "ms (" << memcpyString << ")" << Util::Stream::PrintWriter::endl;
    }

    if (Util::Cpu::CpuId::getCpuFeatures().contains(Util::Cpu::CpuId::SSE)) {
        uint32_t memsetSseResult, memcpySseResult;
        Util::System::out << "Running memory benchmarks with SSE..." << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        benchmark(iterations, Util::Memory::SseAddress<uint32_t>(buffer1), Util::Memory::SseAddress<uint32_t>(buffer2), memsetSseResult, memcpySseResult);

        double memsetSpeedup = (double) memsetResult / memsetSseResult;
        double memcpySpeedup = (double) memcpyResult / memcpySseResult;
        auto memsetString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memsetSpeedup), static_cast<uint32_t>((memsetSpeedup - static_cast<uint32_t>(memsetSpeedup)) * 100));
        auto memcpyString = Util::Memory::String::format("%u.%02ux", static_cast<uint32_t>(memcpySpeedup), static_cast<uint32_t>((memcpySpeedup - static_cast<uint32_t>(memcpySpeedup)) * 100));

        resultWriter << Util::Stream::PrintWriter::endl << "SSE enabled:" << Util::Stream::PrintWriter::endl
                     << "memset: " << memsetSseResult << "ms (" << memsetString << ")" << Util::Stream::PrintWriter::endl
                     << "memcpy: " << memcpySseResult << "ms (" << memcpyString << ")" << Util::Stream::PrintWriter::endl;
    }
    
    delete[] buffer1;
    delete[] buffer2;

    Util::System::out << Util::Stream::PrintWriter::endl << resultStream.getContent() << Util::Stream::PrintWriter::flush;
    return 0;
}
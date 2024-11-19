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

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/base/Address.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/math/Random.h"
#include "lib/interface.h"
#include "lib/util/base/Constants.h"

const constexpr uint8_t BENCHMARK_REPETITIONS = 10;

Util::Math::Random random;

Util::Time::Timestamp benchmarkMemset(const Util::Address<uint32_t> &address, uint32_t length) {
    auto value = static_cast<uint32_t>(random.nextRandomNumber() * 0xff);

    auto start = Util::Time::getSystemTime();
    address.setRange(value, length);
    return Util::Time::getSystemTime() - start;
}

Util::Time::Timestamp benchmarkMemcpy(const Util::Address<uint32_t> &source, const Util::Address<uint32_t> &target, uint32_t length) {
    auto start = Util::Time::getSystemTime();
    target.copyRange(source, length);
    return Util::Time::getSystemTime() - start;
}

Util::String powerAsString(uint8_t power) {
    auto bytes = 1 << power;
    if (power < 10) {
        return Util::String::format("%d B", bytes);
    } else if (power < 20) {
        return Util::String::format("%d KiB", bytes >> 10);
    } else if (power < 30) {
        return Util::String::format("%d MiB", bytes >> 20);
    } else {
        return Util::String::format("%d GiB", bytes >> 30);
    }
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Memory bandwidth benchmark comparing different acceleration techniques.\n"
                               "Each iteration operates on 1 MiB of memory (Default: 100 iterations).\n"
                               "Usage: membench [memset/memcpy] [Minimimum power of 2] [Maximum power of 2]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "membench: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    const auto &benchmarkType = arguments[0];
    const uint8_t minPower = arguments.length() > 1 ? Util::String::parseInt(static_cast<const char*>(arguments[1])) : 10;
    const uint8_t maxPower = arguments.length() > 2 ? Util::String::parseInt(static_cast<const char*>(arguments[2])) : 24;

    if (benchmarkType == "memset" || benchmarkType == "memcpy") {
        for (uint8_t i = minPower; i <= maxPower; i++) {
            auto size = 1 << i;
            auto results = Util::Array<Util::Time::Timestamp>(BENCHMARK_REPETITIONS);

            Util::System::out << benchmarkType << " " << powerAsString(i) << ":\t" << Util::Io::PrintStream::flush;

            if (benchmarkType == "memset") {
                // Allocate buffer
                auto buffer = ::allocateMemory(size, Util::PAGESIZE);
                auto address = Util::Address<uint32_t>(buffer);

                // Warmup round to make sure the memory is mapped
                benchmarkMemset(address, size);

                for (uint32_t j = 0; j < BENCHMARK_REPETITIONS; j++) {
                    results[j] = benchmarkMemset(address, size);
                }
            } else {
                // Allocate source and target buffer
                auto source = ::allocateMemory(size, Util::PAGESIZE);
                auto target = ::allocateMemory(size, Util::PAGESIZE);
                auto sourceAddress = Util::Address<uint32_t>(source);
                auto targetAddress = Util::Address<uint32_t>(target);

                // Warmup round to make sure the memory is mapped
                sourceAddress.setRange(static_cast<uint8_t>(random.nextRandomNumber() * 0xff), size);
                benchmarkMemcpy(sourceAddress, targetAddress, size);

                for (uint32_t j = 0; j < BENCHMARK_REPETITIONS; j++) {
                    results[j] = benchmarkMemcpy(sourceAddress, targetAddress, size);
                }
            }

            uint64_t sum = 0;
            for (auto result : results) {
                sum += result.toNanoseconds();
            }

            auto averageNanos = sum / BENCHMARK_REPETITIONS;
            auto averageSeconds = averageNanos / 1000000000.0;
            auto bandwidth = size / averageSeconds / 1000000.0;

            Util::System::out.setDecimalPrecision(9);
            Util::System::out << averageSeconds << "s (" << Util::Io::PrintStream::flush;
            Util::System::out.setDecimalPrecision(2);
            Util::System::out << bandwidth << " MB/s)" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        }
    } else {
        Util::System::error << "membench: Invalid benchmark type!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    return 0;
}
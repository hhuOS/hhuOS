/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <stdint.h>

#include <interface.h>
#include <lib/util/base/Constants.h>
#include <util/base/Address.h>
#include <util/base/ArgumentParser.h>
#include <util/base/String.h>
#include <util/base/System.h>
#include <util/collection/Array.h>
#include <util/io/stream/PrintStream.h>
#include <util/math/Random.h>
#include <util/time/Timestamp.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

constexpr uint8_t BENCHMARK_REPETITIONS = 10;

Util::Math::Random random;

/// Perform a single benchmark run of memset and return the time taken in nanoseconds.
/// The memory described by the address and length parameters will be filled with a random byte value.
Util::Time::Timestamp benchmarkMemset(const Util::Address &address, const size_t length) {
    const auto value = static_cast<uint8_t>(random.getRandomNumber(0, UINT8_MAX));

    const auto start = Util::Time::Timestamp::getSystemTime();
    address.setRange(value, length);
    return Util::Time::Timestamp::getSystemTime() - start;
}

/// Perform a single benchmark run of memcpy and return the time taken in nanoseconds.
/// The memory described by the source address and length parameters will be copied to the target address.
Util::Time::Timestamp benchmarkMemcpy(const Util::Address &source, const Util::Address &target, const size_t length) {
    const auto start = Util::Time::Timestamp::getSystemTime();
    target.copyRange(source, length);
    return Util::Time::Timestamp::getSystemTime() - start;
}

/// Convert a power of two to a human-readable string with the appropriate unit (B, KiB, MiB, GiB).
/// For example, a power of 10 will be converted to "1 KiB", a power of 20 will be converted to "1 MiB", etc.
///
/// ```c++
/// const auto str = powerAsString(20); // str will be "1 MiB"
/// const auto str = powerAsString(30); // str will be "1 GiB"
/// const auto str = powerAsString(15); // str will be "32 KiB"
/// const auto str = powerAsString(5);  // str will be "32 B"
/// const auto str = powerAsString(0);  // str will be "1 B"
/// ```
Util::String powerAsString(const uint8_t power) {
    const auto bytes = 1 << power;

    if (power < 10) {
        return Util::String::format("%d B", bytes);
    }

    if (power < 20) {
        return Util::String::format("%d KiB", bytes >> 10);
    }

    if (power < 30) {
        return Util::String::format("%d MiB", bytes >> 20);
    }

    return Util::String::format("%d GiB", bytes >> 30);
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "membench: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto &benchmarkType = arguments[0];
    if (benchmarkType != "memset" && benchmarkType != "memcpy") {
        Util::System::error << "membench: Invalid benchmark type!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const uint8_t minPower = arguments.length() > 1 ? Util::String::parseNumber<uint8_t>(arguments[1]) : 10;
    const uint8_t maxPower = arguments.length() > 2 ? Util::String::parseNumber<uint8_t>(arguments[2]) : 24;

    for (uint8_t i = minPower; i <= maxPower; i++) {
        const auto size = 1 << i;
        Util::Array<Util::Time::Timestamp> results(BENCHMARK_REPETITIONS);

        Util::System::out << benchmarkType << " " << powerAsString(i) << ":\t" << Util::Io::PrintStream::flush;

        if (benchmarkType == "memset") {
            // Allocate buffer
            const auto buffer = allocateMemory(size, Util::PAGESIZE);
            auto address = Util::Address(buffer);

            // Warmup round to make sure the memory is mapped
            benchmarkMemset(address, size);

            for (size_t j = 0; j < BENCHMARK_REPETITIONS; j++) {
                results[j] = benchmarkMemset(address, size);
            }

            freeMemory(buffer);
        } else {
            // Allocate source and target buffer
            const auto source = allocateMemory(size, Util::PAGESIZE);
            const auto target = allocateMemory(size, Util::PAGESIZE);
            auto sourceAddress = Util::Address(source);
            auto targetAddress = Util::Address(target);

            // Warmup round to make sure the memory is mapped
            sourceAddress.setRange(static_cast<uint8_t>(random.getRandomNumber(0, UINT8_MAX)), size);
            benchmarkMemcpy(sourceAddress, targetAddress, size);

            for (size_t j = 0; j < BENCHMARK_REPETITIONS; j++) {
                results[j] = benchmarkMemcpy(sourceAddress, targetAddress, size);
            }

            freeMemory(source);
            freeMemory(target);
        }

        uint64_t sum = 0;
        for (auto result : results) {
            sum += result.toNanoseconds();
        }

        const auto averageNanos = sum / BENCHMARK_REPETITIONS;
        const auto averageSeconds = static_cast<double>(averageNanos) / 1000000000.0;
        const auto bandwidth = size / averageSeconds / 1000000.0;

        Util::System::out.setDecimalPrecision(9);
        Util::System::out << averageSeconds << "s (" << Util::Io::PrintStream::flush;
        Util::System::out.setDecimalPrecision(2);
        Util::System::out << bandwidth << " MB/s)" << Util::Io::PrintStream::lnFlush;
    }

    return 0;
}
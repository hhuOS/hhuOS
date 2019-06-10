/*
 * Copyright (C) 2018/19 Thiemo Urselmann
 * Heinrich-Heine University
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#include "kernel/service/NetworkService.h"
#include "kernel/core/SystemManagement.h"
#include "kernel/service/TimeService.h"
#include "NetworkTest.h"

NetworkTest::NetworkTest(Shell &shell) : Command(shell) {

};

void NetworkTest::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addParameter("send", "s", false);
    parser.addParameter("time", "t", false);
    parser.addParameter("count", "c", false);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *networkService = Kernel::getService<NetworkService>();

    if(networkService->getDeviceCount() == 0) {
        stderr << args[0] << ": No network devices available!" << endl;
        return;
    }

    NetworkDevice &driver = Kernel::getService<NetworkService>()->getDriver(0);

    if(!parser.getNamedArgument("send").isEmpty()) {
        int length = static_cast<uint32_t>(strtoint((char *) parser.getNamedArgument("send")));

        if (length <= 0) {
            stderr << "Invalid packet size!" << endl;
            return;
        }

        //one buffer for all packets
        auto *sendBuffer = static_cast<uint8_t *>(SystemManagement::getInstance().mapIO(static_cast<uint32_t>(length)));
        void *phyAddress = SystemManagement::getInstance().getPhysicalAddress(sendBuffer);

        for(int i = 0; i < length; i++) {
            //fill packet with ascending numbers
            *(sendBuffer + i) = static_cast<uint8_t>(i % 16);
        }

        if(!parser.getNamedArgument("time").isEmpty()) {
            int millis = strtoint((char *) parser.getNamedArgument("time"));

            if (millis < 0) {
                stderr << "Invalid amount of time!" << endl;
                return;
            }

            timeSendBenchmark(phyAddress, length, driver, static_cast<uint32_t>(millis));
        } else if(!parser.getNamedArgument("count").isEmpty()) {
            int count = strtoint((char *) parser.getNamedArgument("count"));

            if (count < 0) {
                stderr << "Invalid amount of packets!" << endl;
                return;
            }

            packetSendBenchmark(phyAddress, length, driver, static_cast<uint32_t>(count));
        } else {
            driver.sendPacket(phyAddress, (uint16_t) length);

            stdout << "Sent a single packet of size " << length << " Bytes." << endl;
        }

        SystemManagement::getInstance().freeIO(sendBuffer);
    }
}

void NetworkTest::timeSendBenchmark(void *physPacketAddress, int packetLength, NetworkDevice &driver, uint32_t millis) {
    uint32_t count = 0;

    auto *timeService = Kernel::getService<TimeService>();
    uint32_t start = timeService->getSystemTime();

    while((timeService->getSystemTime()) < start + millis) {
        driver.sendPacket(physPacketAddress, (uint16_t) packetLength);
        count++;
    }

    double bytesPerSec = 1000 * (((double) count * (double) packetLength) / ((double) millis));

    stdout << "Sent " << count << " packets in " << millis / 1000 << (((millis / 1000) == 1) ? " second." : " seconds.") << endl;
    stdout << "Data throughput: " << (uint32_t) (bytesPerSec / 1000000L) << "MB/s"
           << " (" << (uint32_t) (bytesPerSec / 1048576L) << "MiB/s)" << endl;
}

void NetworkTest::packetSendBenchmark(void *physPacketAddress, int packetLength, NetworkDevice &driver, uint32_t count) {
    auto *timeService = Kernel::getService<TimeService>();
    uint32_t start = timeService->getSystemTime();

    for(uint32_t i = 0; i < count; i++) {
        driver.sendPacket(physPacketAddress, (uint16_t) packetLength);
    }

    uint32_t millis = timeService->getSystemTime() - start;

    double bytesPerSec = 1000 * (((double) count * (double) packetLength) / ((double) millis));

    stdout << "Sent " << count << " packets in " << millis / 1000 << (((millis / 1000) == 1) ? " second" : " seconds.") << endl;
    stdout << "Data throughput: " << (uint32_t) (bytesPerSec / 1000000L) << "MB/s"
           << " (" << (uint32_t) (bytesPerSec / 1048576L) << "MiB/s)" << endl;
}

const String NetworkTest::getHelpText() {
    return "Small utility for testing network devices and measuring network send performance.\n\n"
           "Usage: nettest [OPTIONS]...\n\n"
           "Options:\n"
           "  -s, --send: Send packets via the first network device.\n"
           "    -t, --time: Suboption for --send - Run a send benchmark for a given amount of milliseconds\n"
           "    -c, --count: Suboption fo --send - Run a send benchmark with a given amount of packets\n"
           "  -h, --help: Show this help-message.";
}

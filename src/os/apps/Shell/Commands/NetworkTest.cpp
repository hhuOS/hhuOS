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

#include <kernel/services/NetworkService.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/services/TimeService.h>
#include "NetworkTest.h"

NetworkTest::NetworkTest(Shell &shell) : Command(shell), eventBuffer(1024), storedSendBuffer(1024) {
    Kernel::getService<EventBus>()->subscribe(*this, ReceiveEvent::TYPE);
};

NetworkTest::~NetworkTest() {
    Kernel::getService<EventBus>()->unsubscribe(*this, ReceiveEvent::TYPE);
}

void NetworkTest::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);
    parser.addSwitch("info", "i");
    parser.addSwitch("help", "h");
    parser.addSwitch("receive", "r");
    parser.addParameter("bufferedSend", "b", false);
    parser.addParameter("transmit", "t", false);
    parser.addParameter("flood", "f", false);


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

    int length;
    uint8_t *sendBuffer;
    void *phyAddress;

    if (parser.checkSwitch("info")) {
        printInfo();
    } else if(!parser.getNamedArgument("bufferedSend").isEmpty()) {
        int packets = strtoint((char *) parser.getNamedArgument("bufferedSend"));
        if ( packets < 0) {
            stderr << "Invalid amount of packets" << endl;
            return;
        }

        sendFromBuffer(packets, driver);
    } else if(!parser.getNamedArgument("transmit").isEmpty()) {
        length =  strtoint((char *) parser.getNamedArgument("transmit"));
        if (length < 0) {
            stderr << "Invalid packet length" << endl;
            return;
        }

        //one buffer for all packets
        sendBuffer = (uint8_t *) SystemManagement::getInstance().mapIO(8u * length);
        phyAddress = SystemManagement::getInstance().getPhysicalAddress(sendBuffer);

        for(int i = 0; i < length; i++) {
            //fill packets with unique numbers. (16 * 0, 16 * 1,...,16 * 15)+
            *(sendBuffer + i) = (uint8_t) ((i / 16) % 16);
        }

        if(!parser.getNamedArgument("flood").isEmpty()) {
            int millis = strtoint((char *) parser.getNamedArgument("flood"));
            if (millis < 0) {
                stderr << "Invalid amount of time" << endl;
                return;
            }

            int throughput = measurePackets(length, driver, phyAddress, millis);
            stdout << "Transmitted packets total: ";
            stdout << transmitted;
            stdout << endl;
            stdout << "Data throughtput: ";
            stdout << throughput;
            stdout << " Mb/s";
            stdout << endl;
        } else {
            driver.sendPacket(phyAddress, (uint16_t) length);
            if(transmitted < (1u << 31u)) transmitted++;
            stdout << "Packet of length ";
            stdout << length;
            stdout << " bytes send";
            stdout << endl;
        }
        SystemManagement::getInstance().freeIO(sendBuffer);
    } else if (parser.checkSwitch("receive")) {
        printRingBufferPacket();
    }
}

void NetworkTest::printRingBufferPacket() {
    ReceiveEvent event;

    lock.acquire();
    if(eventBuffer.isEmpty()) {
        stdout << "No new packets received";
        stdout << endl;
        //next release is at the end of this method!
        lock.release();
        return;
    } else {
        event = eventBuffer.pop();
    }

    if(read < (1u << 31u)) read++;

    auto *packet = static_cast<uint8_t *>(event.getPacket());
    uint16_t packetLength = event.getLength();

    //current holds first element of the packet. The
    //packet contains char-values.
    uint8_t current = *(packet);
    stdout << "Received paket of length: ";
    stdout << packetLength;
    stdout << endl;

    for(int i = 0; i < packetLength; i++) {
        //convert char value so that its hex will be displayed
        //when using with stdout
        uint8_t posl = current >> 4u;
        uint8_t posr = (uint8_t) (current & 0x0Fu);

        posl > 9 ? posl += 55 : posl += 48;
        posr > 9 ? posr += 55 : posr += 48;

        stdout << posl;
        stdout << posr;

        //align the output to look similar to wiresharks output
        if((i + 1) % 16 == 0)
            stdout << endl;
        else if ((i + 1) % 8 == 0)
            stdout << "  ";
        else
            stdout << " ";

        packet++;
        current = *packet;
    }
    stdout << endl;
    lock.release();
}

void NetworkTest::sendFromBuffer(int packets, NetworkDevice &driver) {
    ReceiveEvent event;
    uint8_t *packet;
    uint16_t packetLength;

    lock.acquire();
    for(uint8_t i = 0; i < packets; i++) {
        if(storedSendBuffer.isEmpty()) {
            //next release will be at the end of this method
            lock.release();
            return;
        } else {
            event = storedSendBuffer.pop();
        }

        packet = static_cast<uint8_t *>(event.getPacket());
        packetLength = event.getLength();

        //remove source, destination and type of the stored packet
        for(int j = 0; j < 14; j++) {
            *(packet + j) = 0;
        }

        //write own MAC-address to the source-field
        driver.getMacAddress(const_cast<uint8_t *>(packet));

        auto physical = (uint64_t *) SystemManagement::getInstance().getPhysicalAddress((uint8_t *) packet);
        driver.sendPacket(physical, packetLength);

        storedSend++;
    }
    lock.release();
}

int NetworkTest::measurePackets(int length, NetworkDevice &driver, void *phyAddress, int millis) {
    uint16_t packetCount = 0;

    auto *timeService = Kernel::getService<TimeService>();
    uint32_t start = timeService->getSystemTime();


    while((timeService->getSystemTime()) < start + millis) {
        driver.sendPacket(phyAddress, (uint16_t) length);
        packetCount++;

        if(transmitted < (1u << 31u)) transmitted++;
    }

    //convert amount of packets counted in given time to Mb/s
    double bytesPerSec = 1000 * ((packetCount * length) / ((double) millis));
    return (int) (bytesPerSec / 1000000);
}

void NetworkTest::printInfo() {
    stdout << "Packets received total: ";
    lock.acquire();
    stdout << received;
    stdout << endl;
    lock.release();
    stdout << "Packets transmitted: ";
    stdout << transmitted;
    stdout << endl;
    stdout << "Packets redirected: ";
    stdout << storedSend;
    stdout << endl;
    stdout << "Packets read: ";
    stdout << read;
    stdout << endl;
}

const String NetworkTest::getHelpText() {
    return "Tests packet transmit and receive for Intel82541IP\n\n"
           "Usage:\n"
           "-h : Displays help text\n"
           "-i : Displays current state\n"
           "-r : Displays the earliest received packet and removes it from a\n"
           "     'receive' buffer, where received packets are stored.\n"
           "-t [size] -f [time] : Sends for time ms packets of the given size in bytes.\n"
           "                      time must be positive and size > 63.\n"
           "-b [amount] : Sends an amount of received packets and \n"
           "              removes them from a 'received and send' buffer, where received packets\n"
           "              are stored. The earliest received packet goes first.\n";
}

void NetworkTest::onEvent(const Event &event) {
    if(event.getType() == ReceiveEvent::TYPE) {
        lock.acquire();
        eventBuffer.push((ReceiveEvent &) event);
        storedSendBuffer.push((ReceiveEvent &) event);


        if(received < (1u << 31u)) {
            received++;
        }
        lock.release();
    }
}

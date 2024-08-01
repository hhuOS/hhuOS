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
 *
 * The nettest application is based on a bachelor's thesis, written by Marcel Thiel.
 * The original source code can be found here: https://github.com/Spectranis/bachelorthesis-mthiel/tree/ne2k_dev
 */

#include <stdint.h>

#include "lib/util/base/System.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "lib/util/network/Socket.h"
#include "lib/util/network/udp/UdpDatagram.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"

static const constexpr uint32_t DEFAULT_BIND_PORT = 1797;
static const constexpr uint32_t DEFAULT_REMOTE_PORT = 1856;
static const constexpr uint32_t DEFAULT_PACKET_SIZE = 1024;
static const constexpr uint32_t DEFAULT_INTERVAL = 10;

/**
 * Receive traffic server mode / client revers mode
 * @param socket
 * @return
 */
int32_t receiveTraffic(Util::Network::Socket &socket){
    uint32_t packetsReceived = 0;
    uint32_t packetsOutOfOrder = 0;
    uint32_t duplicatedPackets = 0;
    uint32_t bytesReceived = 0;
    uint32_t currentPacketNumber;
    uint32_t previousPacketNumber;
    uint32_t intervalCounter = 0;
    uint32_t bytesReceivedInInterval = 0;

    auto firstReceivedDatagram = Util::Network::Udp::UdpDatagram();
    if (!socket.receive(firstReceivedDatagram)) {
        Util::System::error << "nettest: Failed to receive echo request!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::System::out   << "Start: " << Util::Time::getSystemTime().toSeconds() << "s" << Util::Io::PrintStream::endl
                        << "----------------------------------------------" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    /** Start time with first received packet */
    Util::Time::Timestamp secondsPassed = Util::Time::getSystemTime();
    secondsPassed += Util::Time::Timestamp::ofSeconds(1);

    Util::String receivedMessage = Util::String(firstReceivedDatagram.getData(), firstReceivedDatagram.getLength());
    packetsReceived++;
    previousPacketNumber = (receivedMessage[0] << 24) + (receivedMessage[1] << 16) + (receivedMessage[2] << 8) + receivedMessage[3];
    bytesReceivedInInterval = firstReceivedDatagram.getLength();

    /** Receive Packets until exit is send */
    while (true) {
        auto receivedDatagram = Util::Network::Udp::UdpDatagram();
        if (!socket.receive(receivedDatagram)) {
            Util::System::error << "nettest: Failed to receive echo request!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }
        receivedMessage = Util::String(receivedDatagram.getData(), receivedDatagram.getLength());

        /** If message equals exit: break loop
         *  Currently Max Number of Packets: 1.702.390.132 as this equals exit
         */
        if (receivedMessage.strip() == "exit") {
            break;
        }
        packetsReceived++;
        currentPacketNumber = (receivedMessage[0] << 24) + (receivedMessage[1] << 16) + (receivedMessage[2] << 8) + receivedMessage[3];
        /** Check if packet is duplicated*/
        if (currentPacketNumber == previousPacketNumber){
            duplicatedPackets++;
            /** Check if the currentPacketNumber matches previous + 1 */
        } else if (currentPacketNumber != (previousPacketNumber + 1) || currentPacketNumber < previousPacketNumber){
            packetsOutOfOrder++;
        }
        previousPacketNumber = currentPacketNumber;
        bytesReceivedInInterval = bytesReceivedInInterval + receivedDatagram.getLength();

        /** if a second passed write current bytes per second into output */
        if (secondsPassed < Util::Time::getSystemTime()) {
            Util::System::out << intervalCounter << "-" << intervalCounter + 1 << ":    " << bytesReceivedInInterval / 1000 << " KB/s" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            intervalCounter++;
            bytesReceived = bytesReceived + bytesReceivedInInterval;
            /** reset bytes received */
            bytesReceivedInInterval = 0;
            /** set seconds to next second passed */
            secondsPassed += Util::Time::Timestamp::ofSeconds(1);
        }
    }
    bytesReceived = bytesReceived + bytesReceivedInInterval;

    Util::System::out   << intervalCounter << "-" << intervalCounter + 1 << ":    " << bytesReceivedInInterval / 1000 << " KB/s" << Util::Io::PrintStream::endl
                        << "Received exit: End reception" << Util::Io::PrintStream::endl
                        << "----------------------------------------------" << Util::Io::PrintStream::endl
                        << "Bytes received         : " << bytesReceived / 1000 << " KB" << Util::Io::PrintStream::endl
                        << "Average Bytes received : " << (bytesReceived / (intervalCounter + 1)) / 1000 << " KB/s" << Util::Io::PrintStream::endl
                        << "Packets out of order   : " << packetsOutOfOrder << "/" << packetsReceived << Util::Io::PrintStream::endl
                        << "Duplicated packets     : " << duplicatedPackets << Util::Io::PrintStream::endl
                        << "----------------------------------------------" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    return 0;
}

/**
 * Send traffic from server/client to remote client/server
 * @param socket
 * @param destinationAddress
 * @param timingInterval
 * @param packetLength
 * @return
 */
int32_t sendTraffic(Util::Network::Socket &socket, const Util::Network::Ip4::Ip4PortAddress &destinationAddress, uint16_t timingInterval, uint16_t packetLength) {
    /** Create packet  */
    auto *packet = new uint8_t [packetLength];
    uint32_t packetNumber = 0;
    uint32_t intervalCounter = 0;
    uint32_t bytesSendInInterval = 0;

    /** First 4 Bytes are PacketNum and are filled in the loop, all others are filled with 0 */
    for (int i = 4; i < packetLength; i++) {
        packet[i] = 0;
    }

    /** Set Interval End */
    Util::Time::Timestamp testFinishTime = Util::Time::getSystemTime();
    Util::Time::Timestamp secondsPassed = Util::Time::getSystemTime();
    testFinishTime += Util::Time::Timestamp::ofSeconds(timingInterval);
    secondsPassed += Util::Time::Timestamp::ofSeconds(1);

    Util::System::out   << "Start: " << Util::Time::getSystemTime().toSeconds() << "s - End: " << testFinishTime.toSeconds() << "s" << Util::Io::PrintStream::endl
                        << "----------------------------------------------" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    /** Send Packets until finish time's reached */
    while (testFinishTime > Util::Time::getSystemTime()) {
        /** Increment packet number */
        packetNumber++;
        /** Write current Packet number into first 4 Bytes of Buffer*/
        packet[0] = (packetNumber >> 24) & 0xff;
        packet[1] = (packetNumber >> 16) & 0xff;
        packet[2] = (packetNumber >> 8) & 0xff;
        packet[4] = packetNumber & 0xff;

        auto transmitDatagram = Util::Network::Udp::UdpDatagram(static_cast<const uint8_t*>(packet), packetLength, destinationAddress);

        /** transmit packet to server */
        if (!socket.send(transmitDatagram)) {
            Util::System::error << "nettest: Failed to send throughput test packet!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        /** track Bytes send within interval */
        bytesSendInInterval = bytesSendInInterval + packetLength;

        /** if a second has passed write current Bytes per second into output */
        if (secondsPassed < Util::Time::getSystemTime()) {
            Util::System::out << intervalCounter << "-" << intervalCounter + 1 << ":    " << bytesSendInInterval / 1000 << " KB/s" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            intervalCounter++;
            /** reset Bytes send */
            bytesSendInInterval = 0;
            /** set seconds to next second passed */
            secondsPassed += Util::Time::Timestamp::ofSeconds(1);
        }
    }

    /** Send Exit Msg to Server */
    Util::String exitString = "exit";
    auto sendDatagram = Util::Network::Udp::UdpDatagram(static_cast<const uint8_t*>(exitString), exitString.length(), destinationAddress);

    if (!socket.send(sendDatagram)) {
        Util::System::error << "nettest: Failed to send message to server!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    uint32_t sendBytes = packetLength * packetNumber;
    Util::System::out   << "----------------------------------------------"<< Util::Io::PrintStream::endl
                        << "Packets transmitted : " << packetNumber << Util::Io::PrintStream::endl
                        << "Bytes transmitted   : " << sendBytes / 1000 << " KB" << Util::Io::PrintStream::endl
                        << "Average             : " << (sendBytes / timingInterval) / 1000 << " KB/s" << Util::Io::PrintStream::endl
                        << "----------------------------------------------"<< Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    return 0;
}


/** Server Mode */
int32_t server(Util::Network::Socket &socket) {
    auto localAddress = Util::Network::Ip4::Ip4PortAddress();

    if (!socket.getLocalAddress(localAddress)) {
        Util::System::error << "nettest: Failed to query socket address!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::System::out << "nettest: sever listening on " << localAddress.toString() << "! Send 'exit' to leave." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    /** Wait for client to initiate connection, return if exit code is != 0 */
    while (true) {
        auto receivedDatagram = Util::Network::Udp::UdpDatagram();
        if (!socket.receive(receivedDatagram)) {
            Util::System::error << "nettest: Failed to receive echo request!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        /** If connection request is received: send reply to client */
        if (Util::String(receivedDatagram.getData(), receivedDatagram.getLength()).strip() == "Init") {
            if (!socket.send(receivedDatagram)) {
                Util::System::error << "nettest: Failed to send echo reply!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }

            return receiveTraffic(socket);
        } else if (Util::String(receivedDatagram.getData(), receivedDatagram.getLength()).strip() == "InitR") { /** Reverse test: */
            if (!socket.send(receivedDatagram)) {
                Util::System::error << "nettest: Failed to send echo reply!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }

            /** Wait for message with packetLength and timing interval */
            if (!socket.receive(receivedDatagram)) {
                Util::System::error << "nettest: Failed to receive echo request!" << Util::Io::PrintStream::endl
                                    << Util::Io::PrintStream::flush;
                return -1;
            }
            if(receivedDatagram.getLength() != 4){
                Util::System::error << "nettest: Failed to receive reverse test data! " << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }

            /** Get packetLen and Test duration */
            auto data = receivedDatagram.getData();
            uint16_t packetLength = (data[0] << 8) + data[1];
            uint16_t timingInterval = (data[2] << 8) + data[3];

            /** Get destination address from client */
            auto destinationAddress = reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress&>(receivedDatagram.getRemoteAddress());
            destinationAddress.setPort(receivedDatagram.getRemotePort());
            /** Start reverse test */
            return sendTraffic(socket, destinationAddress, timingInterval, packetLength);
        }
    }
}


/** Client Mode */
int32_t client(Util::Network::Socket &socket, const Util::Network::Ip4::Ip4PortAddress &destinationAddress, uint16_t timingInterval, uint16_t packetLength, bool reverseTest) {
    /** Prepare init message */
    Util::String initMsg = "Init";
    if (reverseTest) {
        initMsg = "InitR";
        Util::System::out << "Init reverse test connection" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    } else {
        Util::System::out << "Init test connection" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    /** Send Init message to Server */
    auto sendDatagram = Util::Network::Udp::UdpDatagram(static_cast<const uint8_t*>(initMsg), initMsg.length(), destinationAddress);
    if (!socket.send(sendDatagram)) {
        Util::System::error << "nettest: Failed to send message to server!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    /** Wait for reply from server */
    Util::System::out << "Waiting for Server reply" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    auto receivedDatagram = Util::Network::Udp::UdpDatagram();
    if (!socket.receive(receivedDatagram)) {
        Util::System::error << "nettest: Failed to receive server reply!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    /** Check if Server response is correct */
    auto message = Util::String(receivedDatagram.getData(), receivedDatagram.getLength());
    if (!(message == initMsg)) {
        Util::System::error << "nettest: Server replied with wrong message!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    /** If reverse test true send test duration and packet length to receiver */
    if (reverseTest) {
        auto *buffer = new uint8_t [4];
        buffer[0] = timingInterval >> 8;
        buffer[1] = timingInterval &0xFF;
        buffer[2] = packetLength >> 8;
        buffer[3] = packetLength &0xFF;

        auto reverseTestDatagram = Util::Network::Udp::UdpDatagram(static_cast<const uint8_t*>(buffer), 4, destinationAddress);
        if (!socket.send(reverseTestDatagram)) {
            Util::System::error << "nettest: Failed to send message to server!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }

        return receiveTraffic(socket);
    } else {
        /** else: normal test transmit packets to server */
        return sendTraffic(socket, destinationAddress, timingInterval, packetLength);
    }
}

/** Initial design taken from application uecho in hhuOS/src/application/uecho
 * Nettest application is inspired by iperf3
 */
int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.addSwitch("server", "s");
    argumentParser.addArgument("client", false, "c");
    argumentParser.addArgument("time", false,  "t");
    argumentParser.addArgument("remote", false,  "r");
    argumentParser.addArgument("packetLength", false,  "p");
    argumentParser.addSwitch("reverse",  "R");

    argumentParser.setHelpText("Start a nettest server/client\n"
                               "Usage: nettest [option]\n"
                               "Options:\n"
                               "\n"
                               "Server specific:\n"
                               "-s, --server [ADDRESS]:[PORT] : Run in server mode and bind to [local address:port] \n"
                               "\n"
                               "Client specific:\n"
                               "-c, --client [ADDRESS]:[HOST]: Run in client mode and bind to [local address:port] \n"
                               "-t, --time [uint16_t]: Rest duration in seconds; Default: 10\n"
                               "-r, --remote [ADDRESS]:[HOST] : Remote server address\n"
                               "-p, --packetLength [uint16_t]: Allowed size in Bytes: 64 <= X <= 1450; Default: 1024\n"
                               "-R, --reverse: Reverse test, Receive packets from server"
                               "\n"
                               "Example:\n"
                               "nettest -c 10.0.2.15:1797 -r 1.2.3.4:1797 -p 1450 -t 20"
    );

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if(!argumentParser.checkSwitch("server") && !argumentParser.hasArgument("client")){
        Util::System::error << "nettest: Please specify server/client mode" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto bindAddress = Util::Network::Ip4::Ip4PortAddress();

    /** To debug comment the following out and the next section in */
    if(argumentParser.hasArgument("server")){
        bindAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("server"));
    } else if(argumentParser.hasArgument("client")){
        bindAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("client"));
    }
    if(bindAddress.getPort() == 0) {
        bindAddress.setPort(DEFAULT_BIND_PORT);
    }

    /**
     * For debugging purposes it's usually easier to hardcode the bindAddress.
     * To this just comment the following lines in and the previous lines out
     */
    /*
    uint8_t bind[4] = {10, 0, 2, 15};
    bindAddress.setAddress(bind);
    bindAddress.setPort(1797);
    */

    uint16_t timingInterval = DEFAULT_INTERVAL;
    if (argumentParser.hasArgument("time")) {
        timingInterval = Util::String::parseInt(argumentParser.getArgument("time"));
        if (timingInterval == 0) {
            timingInterval = DEFAULT_INTERVAL;
        }
    }

    auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
    socket.setTimeout(5000);

    if (!socket.bind(bindAddress)) {
        Util::System::error << "nettest: Failed to bind socket!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (argumentParser.checkSwitch("server")) {
        return server(socket);
    } else {
        auto destinationAddress = Util::Network::Ip4::Ip4PortAddress();
        uint16_t packetLength = DEFAULT_PACKET_SIZE;

        /** To debug comment the following out and the next section in */
        destinationAddress = Util::Network::Ip4::Ip4PortAddress(argumentParser.getArgument("remote"));
        /**
         * For debugging purposes it's recommended to hardcode the destinationAddress.
         * To this just comment the following lines in and the other lines out
         */
        /*
        uint8_t dest[4] = {192,168,2, 101};
        destinationAddress.setAddress(dest);
        destinationAddress.setPort(1798);
        */

        if (destinationAddress.getPort() == 0) {
            destinationAddress.setPort(DEFAULT_REMOTE_PORT);
        }

        if(argumentParser.hasArgument("packetLength")){
            packetLength = Util::String::parseInt(argumentParser.getArgument("packetLength"));
            if (packetLength < 64 || packetLength > 1450) {
                Util::System::error << "nettest: Unsupported packet length!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }
        }

        bool reverseTest = false;
        if (argumentParser.checkSwitch("reverse")) {
            reverseTest = true;
        }

        return client(socket, destinationAddress, timingInterval, packetLength, reverseTest);
    }
}

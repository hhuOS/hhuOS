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
 *
 * The nettest application is based on a bachelor's thesis, written by Marcel Thiel.
 * The original source code can be found here: https://github.com/Spectranis/bachelorthesis-mthiel/tree/ne2k_dev
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1856
#define BUFFER_SIZE 2048

int receiver(int serverSocket, sockaddr_in serverAddr, sockaddr_in clientAddr, socklen_t clientAddrLen){
    uint32_t overallReceivedBytes = 0;
    uint32_t lastPacketNumber = 0;
    uint32_t currentPacketNum = 0;
    uint32_t packetsOutOfOrder = 0;
    uint32_t receivedPackets = 0;
    uint32_t duplicatedPackets = 0;
    uint32_t receivedPacketsInInterval = 0;
    uint32_t intervalReceivedBytes = 0;

    uint16_t currentIntervalNumber = 1;

    unsigned char buffer[BUFFER_SIZE];

    std::cout << "------------------------------------------------------" << std::endl;
    /** Acknowledge receipt of the message */
    sendto(serverSocket, "Init", 4, 0, (struct sockaddr*)&clientAddr, clientAddrLen);

    
    /** Wait for first packet to arrive */
    ssize_t dataSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                                    (struct sockaddr*)&clientAddr, &clientAddrLen);
    /** Set Timer to track seconds passed */
    auto currentTime = std::chrono::steady_clock::now();
    auto prevTime = currentTime;
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime);

    intervalReceivedBytes = intervalReceivedBytes + dataSize;
           
    /** Receive and check UDP packets */
    while(true) {
        ssize_t dataSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                                    (struct sockaddr*)&clientAddr, &clientAddrLen);

        currentTime = std::chrono::steady_clock::now();
        elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime);
        uint16_t timePoint = 1;

        intervalReceivedBytes = intervalReceivedBytes + dataSize;
           
        receivedPacketsInInterval++;
        if(elapsedSeconds.count() >= timePoint){
            prevTime = currentTime;
 
            std::cout << " "<< currentIntervalNumber - 1  << "-" << currentIntervalNumber << ":  " << intervalReceivedBytes / 1000 << " KB/s" << std::endl;
            currentIntervalNumber++;
            receivedPacketsInInterval = 0;
            overallReceivedBytes = overallReceivedBytes + intervalReceivedBytes;
            intervalReceivedBytes = 0;
            timePoint++;
        }
            
            
        if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't' ){
            std::cout << " "<< currentIntervalNumber - 1  << "-" << currentIntervalNumber << ":  " << (intervalReceivedBytes - 4) / 1000 << " KB/s" << std::endl;
            overallReceivedBytes = overallReceivedBytes + intervalReceivedBytes - 4;
            break;
        }

        currentPacketNum = (buffer[1] << 24) + (buffer[2] << 16) +(buffer[3] << 8) + buffer[4];
          
        if(currentPacketNum != (lastPacketNumber +1)){
            packetsOutOfOrder ++;
        }
        if(currentPacketNum == lastPacketNumber){
            duplicatedPackets++;
        }
        lastPacketNumber = currentPacketNum;
        receivedPackets++;         
    }
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Received exit: End server" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Bytes received       : " << overallReceivedBytes / 1000 << " KB" << std::endl;
    std::cout << "Average              : " << (overallReceivedBytes / currentIntervalNumber) / 1000 << " KB/s" << std::endl;
    std::cout << "Packets out of order : " << packetsOutOfOrder << "/" << receivedPackets << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    close(serverSocket);
    
    return 0;
}

int sender(int serverSocket, sockaddr_in serverAddr, sockaddr_in clientAddr, socklen_t clientAddrLen){
    uint16_t testInterval;
    uint16_t packetLength;
    uint32_t packetNumber = 1;
    uint32_t currentIntervalNumber = 1;
    unsigned char buffer[BUFFER_SIZE];
    std::cout << "------------------------------------------------------" << std::endl;
    /** Acknowledge receipt of the message */
    sendto(serverSocket, "InitR", 5, 0, (struct sockaddr*)&clientAddr, clientAddrLen);


    /** Wait for packet interval and len to arrive */
    ssize_t dataSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                                    (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    
    testInterval = buffer[0];
    testInterval = (testInterval << 8) + buffer[1];
    packetLength = buffer[2];
    packetLength = (packetLength << 8) + buffer[3];
    
    std::cout << std::dec <<"Test interval: " << testInterval << "seconds \nPacketLength: " << packetLength << std::endl;
    /** Set Timer to track seconds passed */
    auto currentTime = std::chrono::system_clock::now();
    auto endTime = currentTime + std::chrono::seconds(testInterval);
    auto prevTime = currentTime;
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime);
    

    auto *sendBuffer = new uint8_t[packetLength];
    for(uint16_t i = 0; i < packetLength; i++){
        sendBuffer[i] = 0x00;
    }


    uint32_t sendPacketsInInterval = 0;
    uint32_t sendPackets = 0;

    while (std::chrono::system_clock::now() < endTime)
    {
        sendBuffer[0] = (packetNumber >> 24) & 0xFF;
        sendBuffer[1] = (packetNumber >> 16) & 0xFF;
        sendBuffer[2] = (packetNumber >> 8) & 0xFF;
        sendBuffer[3] = packetNumber & 0xFF;

        /** send packets */
        sendto(serverSocket, sendBuffer, packetLength, 0, (struct sockaddr*)&clientAddr, clientAddrLen);
        
        //necessery to not overwhelm qemu
        usleep(10000);

        packetNumber++;
        sendPacketsInInterval++;

        currentTime = std::chrono::system_clock::now();
        elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime);
        uint16_t timePoint = 1;
        
        
        if(elapsedSeconds.count() >= timePoint){
            prevTime = currentTime;
            
            std::cout << " "<< currentIntervalNumber - 1  << "-" << currentIntervalNumber << ":  " << (sendPacketsInInterval * packetLength) / 1000 << " KB/s" << " Packets:" << sendPacketsInInterval << std::endl;
            timePoint++;
            currentIntervalNumber++;
            sendPackets = sendPackets + sendPacketsInInterval;
            sendPacketsInInterval = 0;
        }
    
    }

    sendPackets = sendPackets + sendPacketsInInterval;
    std::cout << " "<< currentIntervalNumber - 1  << "-" << currentIntervalNumber << ":  " << (sendPacketsInInterval * packetLength) / 1000 << " KB/s" << " Packets:" << sendPacketsInInterval << std::endl;
            

    sendto(serverSocket, "exit", 4, 0, (struct sockaddr*)&clientAddr, clientAddrLen);

    uint32_t overallSendBytes = sendPackets * packetLength;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Timing interval reached: End server" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Bytes send          : " << overallSendBytes / 1000 << " KB" << std::endl;
    std::cout << "Average             : " << (overallSendBytes / testInterval) / 1000 << " KB/s" << std::endl;
    std::cout << "Packets send        : " << sendPackets  << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    close(serverSocket);

    return 0;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    unsigned char buffer[BUFFER_SIZE];
    

    /** Create UDP socket*/
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    /** Configure server address 
     * localhost:1798
     */
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    /** Bind the socket to the specified address and port */
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP Server listening on port " << PORT << std::endl;

    /** Receive message from the client specifying the number of packets */
    ssize_t messageSize = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                                   (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    std::cout << "------------------------------------------------------" << std::endl;

    if(buffer[0] == 'I' && buffer[1] == 'n' && buffer[2] == 'i' && buffer[3] == 't'){
        if(buffer[4] != 'R'){
            std::cout << "Received init message from " << inet_ntoa(clientAddr.sin_addr) << " \nStarting test:" << std::endl;
            receiver(serverSocket, serverAddr, clientAddr, clientAddrLen);
        }
        else{
            std::cout << "Received reverse test init message from " << inet_ntoa(clientAddr.sin_addr) << " \nStarting reverse test:" << std::endl;
            sender(serverSocket, serverAddr, clientAddr, clientAddrLen);
        }
        
    }
    else{
        std::cout << "Received no valid init message! Exit" << std::endl;
        
    }
    return 0;
}

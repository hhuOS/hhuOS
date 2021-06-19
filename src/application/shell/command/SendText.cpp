//
// Created by hannes on 13.06.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/applications/EchoServer.h>
#include <kernel/network/NetworkDefinitions.h>
#include <kernel/network/udp/sockets/UDP4ClientSocket.h>
#include <kernel/service/TimeService.h>
#include "SendText.h"

SendText::SendText(Shell &shell) : Command(shell) {

}

void SendText::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *server = new EchoServer(1024);
    stdout << "Starting ECHO server" << endl;
    if(server->start()){
        stderr << "Starting server failed!" << endl;
        delete server;
        return;
    }

    auto *sendSocket = new Kernel::UDP4ClientSocket(
            new IP4Address(127, 0, 0, 1),
            ECHO_PORT_NUMBER
            );

    stdout << "CLIENT: Binding socket for receive" << endl;
    if(sendSocket->bind()){
        stderr << "CLIENT: Binding socket to receive response failed!" << endl;
        sendSocket->close();
        delete sendSocket;
        if (server->stop()) {
            stderr << "Stopping server failed!" << endl;
        }
        delete server;
        return;
    }

    auto *testString = new String("Hello world! Now it works...\0");

    stdout << "CLIENT: Sending text '" << *testString << "' to server" << endl;
    if(sendSocket->send((char *)*testString,testString->length())) {
        stderr << "CLIENT: Error while sending!" << endl;
        sendSocket->close();
        delete sendSocket;
        delete testString;

        stdout << "Stopping ECHO server" << endl;
        if (server->stop()) {
            stderr << "Stopping server failed!" << endl;
        }
        delete server;
        return;
    }

    size_t totalBytesRead = 0;
    auto *response = new char [testString->length() + 1];
    response[testString->length()] = '\0';

    stdout << "CLIENT: Reading response" << endl;
    if(sendSocket->receive(&totalBytesRead, response, testString->length()) ||
        totalBytesRead!=testString->length()
        ){
        stderr << "CLIENT: Receive error or unexpected number of " << totalBytesRead << " bytes received, stopping" << endl;
    } else {
        stdout << "CLIENT: Response was '" << response << "'" << endl;
    }

    stdout << "CLIENT: Closing socket" << endl;
    sendSocket->close();
    delete sendSocket;
    delete testString;
    delete[] response;

    stdout << "Stopping ECHO server" << endl;
    if (server->stop()) {
        stderr << "Stopping server failed!" << endl;
    }
    delete server;
}

const String SendText::getHelpText() {
    return "Utility for testing our UDP/IP protocol stack by sending and receiving a given text\n\n"
           "Usage: sendtext [TEXT]\n"
           "Options:\n"
           "   -h, --help: Show this help-message";
}
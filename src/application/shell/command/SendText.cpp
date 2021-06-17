//
// Created by hannes on 13.06.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/applications/EchoServer.h>
#include <kernel/network/NetworkDefinitions.h>
#include <kernel/network/udp/sockets/UDP4ClientSocket.h>
#include "SendText.h"

SendText::SendText(Shell &shell) : Command(shell) {

}

void SendText::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *server = new EchoServer(0);
    if(server->start()){
        stderr << "Starting server failed!" << endl;
        delete server;
        return;
    }

    auto *testString = new String("Hello world! Now it works...\0");

    auto *localhost = new IP4Address(127, 0, 0, 1);
    stdout << "CLIENT: Sending text '" << *testString << "' to server" << endl;

    auto *sendSocket = new Kernel::UDP4ClientSocket(localhost, ECHO_PORT_NUMBER);
    auto *response = new char [testString->length() + 1];
    response[testString->length()]='\0';

    if(!sendSocket->send((char *)*testString,testString->length())) {
        sendSocket->receive(response, testString->length());
        stdout << "CLIENT: Response was '" << response << "'" << endl;
    } else{
        stderr << "CLIENT: Error while sending!" << endl;
    }
    sendSocket->close();
    delete sendSocket;
    delete testString;
    delete[] response;

    if(server->stop()){
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
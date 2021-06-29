//
// Created by hannes on 13.06.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/applications/EchoServer.h>
#include <kernel/network/udp/sockets/UDP4ClientSocket.h>
#include "SendText.h"

SendText::SendText(Shell &shell) : Command(shell) {

}

void SendText::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addParameter("count", "n", false);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto unnamedArgs = parser.getUnnamedArguments();
    String testString;

    if (unnamedArgs.length() == 0) {
        testString = "Hello world!";
        stdout << "No text given, using '" << testString << "'" << endl;
    } else {
        testString = "";
        for (auto & unnamedArg : unnamedArgs) {
            testString = testString + unnamedArg + " ";
            if (testString.length() > ECHO_INPUT_BUFFER_SIZE) {
                break;
            }
        }
        if (testString.endsWith(" ")) {
            testString[testString.length() - 1] = '\0';
        }
    }

    auto *server = new EchoServer(ECHO_INPUT_BUFFER_SIZE);
    stdout << "Starting ECHO server" << endl;
    if (server->start()) {
        stderr << "Starting server failed!" << endl;
        delete server;
        return;
    }

    auto *sendSocket =
            new Kernel::UDP4ClientSocket(
                    new IP4Address(127, 0, 0, 1), ECHO_PORT_NUMBER
            );

    stdout << "CLIENT: Binding socket for receive" << endl;
    if (sendSocket->bind()) {
        stderr << "CLIENT: Binding socket to receive response failed!" << endl;
        sendSocket->close();
        delete sendSocket;
        if (server->stop()) {
            stderr << "Stopping server failed!" << endl;
        }
        delete server;
        return;
    }

    //Terminate given String to be sure
    testString = testString + '\0';
    size_t stringLength = testString.length();

    //Cap String if too long
    if (stringLength >= (ECHO_INPUT_BUFFER_SIZE)) {
        testString[ECHO_INPUT_BUFFER_SIZE - 1] = '\0';
        stringLength = ECHO_INPUT_BUFFER_SIZE;
    }

    uint8_t rounds = 1;
    auto count = parser.getNamedArgument("count");
    if (!count.isEmpty()) {
        rounds = static_cast<uint8_t>(strtoint((const char *) count));
    }

    for (uint8_t i = 0; i < rounds; i++) {
        stdout << "CLIENT: Sending text '" << testString << "' to server" << endl;
        if (sendSocket->send((char *) testString, stringLength)) {
            stderr << "CLIENT: Error while sending!" << endl;
            sendSocket->close();
            delete sendSocket;

            stdout << "Stopping ECHO server" << endl;
            if (server->stop()) {
                stderr << "Stopping server failed!" << endl;
            }
            delete server;
            return;
        }

        size_t totalBytesRead = 0;
        char response[ECHO_INPUT_BUFFER_SIZE + 1];
        response[ECHO_INPUT_BUFFER_SIZE] = '\0';

        stdout << "CLIENT: Reading response" << endl;
        if (sendSocket->receive(&totalBytesRead, response, stringLength) || totalBytesRead != stringLength) {
            stderr << "CLIENT: Receive error or unexpected number of " << totalBytesRead << " bytes received, stopping"
                   << endl;
        } else {
            stdout << "CLIENT: Response was '" << response << "'" << endl;
        }
    }

    stdout << "CLIENT: Closing socket" << endl;
    sendSocket->close();
    delete sendSocket;

    stdout << "Stopping ECHO server" << endl;
    if (server->stop()) {
        stderr << "Stopping server failed!" << endl;
    }
    delete server;
}

const String SendText::getHelpText() {
    return "Utility for testing our UDP/IP protocol stack by sending and receiving given text via UDP\n\n"
           "Usage: sendtext [OPTION] [TEXT]\n"
           "Options:\n"
           "   -n, --count: Send and receive given text n times\n"
           "   -h, --help: Show this help-message";
}
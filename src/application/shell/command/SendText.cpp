//
// Created by hannes on 13.06.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include "SendText.h"

SendText::SendText(Shell &shell) : Command(shell) {

}

void SendText::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *localhost = new IP4Address(127, 0, 0, 1);
    auto *testString = new String("Hello world!");

    auto *udp4Datagram = new UDP4Datagram(16000,(char*)testString,testString->length());

    auto *eventBus = Kernel::System::getService<Kernel::EventBus>();
    eventBus->publish(
            Util::SmartPointer<Kernel::Event>(
                    new Kernel::UDP4SendEvent(localhost, udp4Datagram)
            )
    );

}

const String SendText::getHelpText() {
    return "Utility for testing our UDP/IP protocol stack by sending and receiving a given text over it\n\n"
           "Usage: sendtext [TEXT]\n"
           "Options:\n"
           "   -h, --help: Show this help-message";
}
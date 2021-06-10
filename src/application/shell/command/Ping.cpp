//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <kernel/network/udp/TransmittableString.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include "Ping.h"

Ping::Ping(Shell &shell) : Command(shell) {

}

void Ping::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *localhost = new IP4Address(127, 0, 0, 1);
//    auto *pingRequest = new ICMP4Echo(42,0);

    //TODO: Move back to original version when testing is done
    auto *eventBus = Kernel::System::getService<Kernel::EventBus>();
//    eventBus->publish(
//            Util::SmartPointer<Kernel::Event>(
//                    new Kernel::ICMP4SendEvent(localhost, pingRequest)
//            )
//    );

    String helloWorld = "Hello world!";
    auto *testString = new TransmittableString(helloWorld.length());
    testString->append((char*)helloWorld, helloWorld.length());

    auto *datagram = new UDP4Datagram(0, testString);

    eventBus->publish(
            Util::SmartPointer<Kernel::Event>(
                    new Kernel::UDP4SendEvent(localhost, datagram)
            )
    );
}

const String Ping::getHelpText() {
    return "Utility for testing our IP protocol stack via ICMP Echo and ICMP Echo Reply\n\n"
           "Usage: ping [OPTION]\n"
           "Options:\n"
           "   -h, --help: Show this help-message";
}
//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <cstdio>
#include "Ping.h"

Ping::Ping(Shell &shell) : Command(shell) {
    eventBus = new Kernel::NetworkEventBus(Kernel::System::getService<Kernel::EventBus>());
    timeService = Kernel::System::getService<Kernel::TimeService>();
}

void Ping::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *localhost = new IP4Address(127, 0, 0, 1);
    uint8_t pingCount = 10;

    for(uint8_t i=0;i<pingCount;i++){
        eventBus->publish(
                new Kernel::ICMP4SendEvent(
                        new IP4Address(localhost),
                        new ICMP4Echo(42,i)
                        )
        );
        timeService->msleep(1000);
    }


}

const String Ping::getHelpText() {
    return "Utility for testing our IP protocol stack via ICMP Echo and ICMP Echo Reply\n\n"
           "Usage: ping [OPTION]\n"
           "Options:\n"
           "   -h, --help: Show this help-message";
}
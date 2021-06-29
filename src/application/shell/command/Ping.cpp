//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <cstdio>
#include <kernel/network/NetworkDefinitions.h>
#include "Ping.h"

Ping::Ping(Shell &shell) : Command(shell) {
    eventBus = new Kernel::NetworkEventBus(Kernel::System::getService<Kernel::EventBus>());
    timeService = Kernel::System::getService<Kernel::TimeService>();
}

void Ping::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addParameter("count", "n", false);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    uint8_t addressBytes[IP4ADDRESS_LENGTH]{127, 0, 0, 1}, numberOfPings = 3;

    auto target = parser.getUnnamedArguments();
    if (target.length() == 1) {
        IP4Address::parseTo(addressBytes, &target[0]);
    }

    auto count = parser.getNamedArgument("count");
    if (!count.isEmpty()) {
        numberOfPings = static_cast<uint8_t>(strtoint((const char *) count));
    }

    for (uint16_t i = 0; i < numberOfPings; i++) {
        eventBus->publish(
                new Kernel::ICMP4SendEvent(
                        new IP4Address(addressBytes),
                        new ICMP4Echo(42, i + (uint16_t) 1)
                )
        );
        timeService->msleep(1000);
    }
}

const String Ping::getHelpText() {
    return "Utility for testing our IP protocol stack via ICMP Echo and ICMP Echo Reply\n\n"
           "Usage: ping [OPTION] [ADDRESS]\n"
           "ADDRESS:\n"
           "   A valid IPv4 address in format [0-255].[0-255].[0-255].[0-255].\n"
           "   Will be 127.0.0.1 (localhost) if empty\n"
           "Options:\n"
           "   -n, --count: Number of pings to send, default is 3\n"
           "   -h, --help: Show this help-message\n";
}
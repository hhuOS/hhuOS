//
// Created by hannes on 20.04.21.
//

#include <lib/libc/printf.h>
#include <kernel/network/NetworkDefinitions.h>
#include "Ip.h"

Ip::Ip(Shell &shell) : Command(shell) {

}

void Ip::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("link", "l");
    parser.addSwitch("address", "a");
    parser.addSwitch("route", "r");

    parser.addSwitch("set", "s");
    parser.addSwitch("unset", "u");
    parser.addSwitch("default", "d");
    parser.addSwitch("no-default", "nd");

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *networkService = Kernel::System::getService<Kernel::NetworkService>();

    if (networkService->getDeviceCount() == 0) {
        stderr << args[0] << ": No network devices available!" << endl;
        return;
    }

    if (parser.checkSwitch("link")) {
        link(networkService);
        return;
    }

    if (parser.checkSwitch("address")) {
        address(networkService, &parser);
        return;
    }

    if (parser.checkSwitch("route")) {
        route(networkService, &parser);
        return;
    }
}

void Ip::link(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print available network links" << endl;

    auto *linkAttributes = new Util::ArrayList<String>();
    if (networkService->collectLinkAttributes(linkAttributes)) {
        stderr << "Could not collect link attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < linkAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": " << linkAttributes->get(i) << endl;
    }

    delete linkAttributes;
}

void Ip::address(Kernel::NetworkService *networkService, Util::ArgumentParser *parser) {
    if (networkService == nullptr || parser == nullptr) {
        printf("No valid network service or parser given! Exit");
        return;
    }

    if (parser->checkSwitch("set")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 3) {
            stderr << "Invalid argument number, please give three arguments: "
                      "[Interface identifier] [IP4Address] [bitCount Netmask]" << endl;
            return;
        }
        uint8_t bitCount, addressBytes[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
        if (IP4Address::parseTo(addressBytes, unnamedArguments[1])) {
            stderr << "Could not parse input " << unnamedArguments[1] << " as IP4Address!" << endl;
            return;
        }

        bitCount = static_cast<uint8_t>(strtoint((const char *) unnamedArguments[2]));
        if (bitCount > 32) {
            stderr << "Invalid bit count for Netmask length, please use values between 0 and 32" << endl;
            return;
        }

        auto *address = new IP4Address(addressBytes);
        auto *mask = new IP4Netmask(bitCount);

        if (networkService->assignIP4Address(unnamedArguments[0], address, mask)) {
            delete address;
            delete mask;

            stderr << "Assigning address for " << unnamedArguments[0] << " failed! See syslog for details" << endl;
        } else {
            stdout << "Assigned address " << address->asString() << " with mask " <<
                   mask->asString() << " to " << unnamedArguments[0] << endl;
        }
        return;
    } else if (parser->checkSwitch("unset")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 1) {
            stderr << "Invalid argument number, please give only one argument: [Interface identifier]" << endl;
            return;
        }

        if (networkService->unAssignIP4Address(unnamedArguments[0])) {
            stderr << "UnAssigning address for '" << unnamedArguments[0] << "' failed! See syslog for details" << endl;
        } else {
            stdout << "Address unAssigned for '" << unnamedArguments[0] << "'" << endl;
        }
        return;
    }

    //Print interface attributes if 'set' switch not active
    stdout << "Print available ip interfaces" << endl;
    auto *interfaceAttributes = new Util::ArrayList<String>();
    if (networkService->collectInterfaceAttributes(interfaceAttributes)) {
        stderr << "Could not collect interface attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < interfaceAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": " << interfaceAttributes->get(i) << endl;
    }

    delete interfaceAttributes;
}

void Ip::route(Kernel::NetworkService *networkService, Util::ArgumentParser *parser) {
    if (networkService == nullptr || parser == nullptr) {
        printf("No valid network service or parser given! Exit");
        return;
    }

    if (parser->checkSwitch("default")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 2) {
            stderr << "Invalid argument number, please give two arguments: "
                      "[Gateway IP4Address] [Link identifier]" << endl;
            return;
        }
        uint8_t addressBytes[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
        if (IP4Address::parseTo(addressBytes, unnamedArguments[0])) {
            stderr << "Could not parse input " << unnamedArguments[0] << " as IP4Address!" << endl;
            return;
        }
        auto *gatewayAddress = new IP4Address(addressBytes);
        if (networkService->setDefaultRoute(gatewayAddress, unnamedArguments[1])) {
            stderr << "Setting default route '" << gatewayAddress->asString() << " via "
                   << unnamedArguments[1] << "' failed! See syslog for details" << endl;
            delete gatewayAddress;
            return;
        } else {
            stdout << "Default route set to '" << gatewayAddress->asString() << " via "
                   << unnamedArguments[1] << "'" << endl;
        }
        return;
    }
    if (parser->checkSwitch("no-default")) {
        if (networkService->removeDefaultRoute()) {
            stderr << "Could not remove default route, see syslog for details" << endl;
        } else {
            stdout << "Default route removed" << endl;
        }
        return;
    }

    stdout << "Print existing ip routes" << endl;

    auto *routeAttributes = new Util::ArrayList<String>();
    if (networkService->collectRouteAttributes(routeAttributes)) {
        stderr << "Could not collect route attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < routeAttributes->size(); i++) {
        stdout << "\nRoute " << i << ": " << routeAttributes->get(i) << endl;
    }

    delete routeAttributes;
}

const String Ip::getHelpText() {
    return "Utility for reading and setting attributes for IP network interfaces\n\n"
           "Usage: ip [OPTIONS]\n"
           "Options:\n"
           "   -l, --link: List all available network links and their attributes\n"
           "   -a, --address: List all available IPv4 interfaces and their attributes\n"
           "             --set [Link Identifier] [IPv4 Address] [IPv4 Netmask Bits]:\n"
           "               Assign given IPv4 address to given link, only defined for --address\n"
           "               Example: 'ip --address --set eth0 192.168.178.1 24'\n"
           "             --unset [Link Identifier]:\n"
           "               Remove IPv4 address from given link, only defined for --address\n"
           "               Example: 'ip --address --unset eth0'\n"
           "   -r, --route: List all available IPv4 routes and their attributes\n"
           "             --default [Gateway IP4Address] [Link identifier]:\n"
           "               Set given route as default one, only defined for --route\n"
           "               Overrides existing one if already defined\n"
           "               Example: 'ip --route --default 192.168.178.1 eth0'\n"
           "             --no-default:\n"
           "               Remove default route from routing table, if existing\n"
           "               Only directly connected hosts can be reached then\n"
           "               Example: 'ip --route --no-default'\n"
           "   -h, --help: Show this help-message";
}

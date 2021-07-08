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
    parser.addSwitch("neighbor", "neigh");

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

    if (parser.checkSwitch("neighbor")) {
        neighbor(networkService);
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
        stdout << "Device " << i << ": " << linkAttributes->get(i) << endl;
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
                      "[Interface Identifier] [IP4Address] [bitCount Netmask]" << endl;
            return;
        }
        uint8_t bitCount, addressBytes[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
        auto *addressAsString = (char *) unnamedArguments[1];
        if (IP4Address::parseTo(addressBytes, addressAsString)) {
            stderr << "Could not parse input " << addressAsString << " as IP4Address!" << endl;
            return;
        }

        auto bitCountAsString = unnamedArguments[2];
        bitCount = static_cast<uint8_t>(strtoint((const char *) bitCountAsString));
        if (bitCount > 32) {
            stderr << "Invalid bit count for Netmask length, please use values between 0 and 32" << endl;
            return;
        }

        auto identifier = unnamedArguments[0];
        auto *address = new IP4Address(addressBytes);
        auto *mask = new IP4Netmask(bitCount);

        if (networkService->assignIP4Address(identifier, address, mask)) {
            delete address;
            delete mask;

            stderr << "Assigning address for '" << identifier << "' failed! See syslog for details" << endl;
        } else {
            stdout << "Assigned address " << address->asChars() << String::format("/%d", bitCount)
                   << " to '" << identifier << "'" << endl;
        }
        return;
    } else if (parser->checkSwitch("unset")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 1) {
            stderr << "Invalid argument number, please give only one argument: [Interface Identifier]" << endl;
            return;
        }

        auto identifier = unnamedArguments[0];
        if (networkService->unAssignIP4Address(identifier)) {
            stderr << "UnAssigning address for '" << identifier << "' failed! See syslog for details" << endl;
        } else {
            stdout << "Address unAssigned for '" << identifier << "'" << endl;
        }
        return;
    }

    //Print interface attributes if 'set' switch not active
    stdout << "Print ip4 addresses" << endl;
    auto *interfaceAttributes = new Util::ArrayList<String>();
    if (networkService->collectInterfaceAttributes(interfaceAttributes)) {
        stderr << "Could not collect interface attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < interfaceAttributes->size(); i++) {
        stdout << "Device " << i << ": " << interfaceAttributes->get(i) << endl;
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
        auto *addressAsString = (char *) unnamedArguments[0];
        if (IP4Address::parseTo(addressBytes, addressAsString)) {
            stderr << "Could not parse input " << addressAsString << " as IP4Address!" << endl;
            return;
        }
        auto *gatewayAddress = new IP4Address(addressBytes);
        auto identifier = unnamedArguments[1];
        if (networkService->setDefaultRoute(gatewayAddress, identifier)) {
            stderr << "Setting default route '" << gatewayAddress->asChars() << " via "
                   << identifier << "' failed! See syslog for details" << endl;
            delete gatewayAddress;
            return;
        } else {
            stdout << "Default route set to '" << gatewayAddress->asChars() << " via "
                   << identifier << "'" << endl;
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

    stdout << "Print ip4 routes" << endl;

    auto *routeAttributes = new Util::ArrayList<String>();
    if (networkService->collectRouteAttributes(routeAttributes)) {
        stderr << "Could not collect route attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < routeAttributes->size(); i++) {
        stdout << "Route " << i << ": " << routeAttributes->get(i) << endl;
    }

    delete routeAttributes;
}

void Ip::neighbor(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print arp tables" << endl;

    auto *arpTables = new Util::ArrayList<String>();
    if (networkService->collectARPTables(arpTables)) {
        stderr << "Could not collect ARP tables, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < arpTables->size(); i++) {
        stdout << "Device " << i << ": " << arpTables->get(i) << endl;
    }

    delete arpTables;
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
           "             --default [Gateway IP4Address] [Link Identifier]:\n"
           "               Set given route as default one, only defined for --route\n"
           "               Overrides existing one if already defined\n"
           "               Example: 'ip --route --default 192.168.178.1 eth0'\n"
           "             --no-default:\n"
           "               Remove default route from routing table, if existing\n"
           "               Only directly connected hosts can be reached then\n"
           "               Example: 'ip --route --no-default'\n"
           "   -neigh, --neighbor: List ARP tables of all registered interfaces\n"
           "   -h, --help: Show this help-message";
}

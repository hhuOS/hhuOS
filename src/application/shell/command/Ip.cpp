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
        route(networkService);
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
    if(networkService->collectLinkAttributes(linkAttributes)){
        stderr << "Could not collect link attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < linkAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": " << linkAttributes->get(i) << endl;
    }

    delete linkAttributes;
}

void Ip::address(Kernel::NetworkService *networkService, Util::ArgumentParser *parser) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    if (parser->checkSwitch("set")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 3) {
            stderr << "Invalid argument number, please give three arguments: "
                      "[Interface identifier] [IP4Address] [bitCount Netmask]" << endl;
            return;
        }
        uint8_t addressBytes[IP4ADDRESS_LENGTH]{0, 0, 0, 0}, bitCount = 0;
        IP4Address::parseTo(addressBytes, &unnamedArguments[1]);

        bitCount = strtoint((const char *) unnamedArguments[2]);
        if (bitCount > 32) {
            stderr << "Invalid bit count for Netmask length, please use values between 0 and 32" << endl;
            return;
        }

        auto *identifier = new EthernetDeviceIdentifier(&unnamedArguments[0]);
        auto *address =new IP4Address(addressBytes);
        auto *mask = new IP4Netmask(bitCount);

        if (networkService->assignIP4Address(identifier,address,mask)) {
            delete identifier;
            delete address;
            delete mask;
            stderr << "Assigning address for " << unnamedArguments[0] << " failed! See syslog for details" << endl;
        } else{
            stdout << "Assigned address " << address->asString() << " with mask " <<
            mask->asString() << " to '" << identifier->asString() << "'" << endl;
        }
        return;
    } else if (parser->checkSwitch("unset")) {
        auto unnamedArguments = parser->getUnnamedArguments();
        if (unnamedArguments.length() != 1) {
            stderr << "Invalid argument number, please give only one argument: [Interface identifier]" << endl;
            return;
        }

        if (networkService->unAssignIP4Address(new EthernetDeviceIdentifier(&unnamedArguments[0]))) {
            stderr << "Assigning address for " << unnamedArguments[0] << " failed! See syslog for details" << endl;
        }
        return;
    }

    //Print interface attributes if 'set' switch not active
    stdout << "Print available ip interfaces" << endl;
    auto *interfaceAttributes = new Util::ArrayList<String>();
    if(networkService->collectInterfaceAttributes(interfaceAttributes)){
        stderr << "Could not collect interface attributes, return" << endl;
        return;
    }

    for (uint32_t i = 0; i < interfaceAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": " << interfaceAttributes->get(i) << endl;
    }

    delete interfaceAttributes;
}

void Ip::route(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print existing ip routes" << endl;

    auto *routeAttributes = new Util::ArrayList<String>();
    if(networkService->collectRouteAttributes(routeAttributes)){
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
           "   -h, --help: Show this help-message";
}

//
// Created by hannes on 20.04.21.
//

#include <lib/libc/printf.h>
#include "Ip.h"

Ip::Ip(Shell &shell) : Command(shell) {

}

void Ip::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("link", "l");
    parser.addSwitch("address", "a");
    parser.addSwitch("route", "r");

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
        address(networkService);
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

    auto * linkAttributes=new Util::ArrayList<String>();
    networkService->collectLinkAttributes(linkAttributes);

    for (uint32_t i=0; i < linkAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": "  << linkAttributes->get(i) << endl;
    }
}

void Ip::address(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print available ip interfaces" << endl;

    auto * interfaceAttributes=new Util::ArrayList<String>();
    networkService->collectInterfaceAttributes(interfaceAttributes);

    for (uint32_t i=0; i < interfaceAttributes->size(); i++) {
        stdout << "\nDevice " << i << ": "  << interfaceAttributes->get(i) << endl;
    }
}

void Ip::route(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print existing ip routes" << endl;

    auto * routeAttributes=new Util::ArrayList<String>();
    networkService->collectRouteAttributes(routeAttributes);

    for (uint32_t i=0; i < routeAttributes->size(); i++) {
        stdout << "\nRoute " << i << ": "  << routeAttributes->get(i) << endl;
    }
}

const String Ip::getHelpText() {
    return "Utility for reading and setting attributes for IP network interfaces\n\n"
           "Usage: ip [OPTIONS]\n"
           "Options:\n"
           "   -l, --link: List all available network links and their attributes\n"
           "   -h, --help: Show this help-message";
}

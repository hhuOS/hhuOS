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
    }
}

void Ip::link(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print available network links" << endl;

    auto *macAddress = (uint8_t *) malloc(6 * sizeof(uint8_t));
    uint32_t count = networkService->getDeviceCount();

    for (uint32_t i = 0; i < count; i++) {
        networkService->getDriver(i).getMacAddress(macAddress);
        stdout << "Link No. " << i << ": MAC='" << getMACAsString(macAddress) << "'" << endl;
    }

    free(macAddress);
}

String Ip::getMACAsString(uint8_t *mac) {
    return String::format("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

const String Ip::getHelpText() {
    return "Utility for reading and setting attributes for IP network interfaces\n\n"
           "Usage: ip [OPTIONS]\n"
           "Options:\n"
           "   -l, --link: List all available network links and their attributes\n"
           "   -h, --help: Show this help-message";
}
//
// Created by hannes on 20.04.21.
//

#ifndef HHUOS_IP_H
#define HHUOS_IP_H


#include "Command.h"
#include <kernel/service/NetworkService.h>
#include <device/network/loopback/Loopback.h>

class Ip : public Command {

private:
    Loopback *loopbackInterface = nullptr;

public:

    Ip() = delete;

    Ip(const Ip &copy) = delete;

    virtual ~Ip();

    explicit Ip(Shell &shell);

    void link(Kernel::NetworkService *networkService);

    void loopback(Kernel::NetworkService *networkService);

    void execute(Util::Array<String> &args) override;

    const String getHelpText() override;

    static String getMACAsString(uint8_t *mac);
};


#endif //HHUOS_IP_H

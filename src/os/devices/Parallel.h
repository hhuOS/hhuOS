#ifndef HHUOS_PARALLEL_H
#define HHUOS_PARALLEL_H

#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>

class Parallel {

public:

    /**
     * Starting addresses of the registers of each port.
     */
    enum LptPort {
        LPT1 = 0x378,
        LPT2 = 0x278,
        LPT3 = 0x3bc,
    };

    explicit Parallel(LptPort port);

    ~Parallel() = default;

    void initializePrinter();

    bool isBusy();

    bool isPaperEmpty();

    bool checkError();

    void sendChar(char c);

    void sendData(char *data, uint32_t len);

    /**
     * Get the port number.
     */
    LptPort getPortNumber();

private:

    TimeService *timeService = nullptr;

    LptPort port;

    IOport dataPort;
    IOport statusPort;
    IOport controlPort;
};

#endif

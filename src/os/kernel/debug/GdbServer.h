//
// Created by krakowski on 08.06.18.
//

#ifndef __GdbServer_include__
#define __GdbServer_include__

#define BREAKPOINT() asm("   int $3");

#include <kernel/threads/ThreadState.h>

class GdbServer {

public:

    GdbServer() = delete;

    GdbServer(const GdbServer &other) = delete;

    GdbServer &operator=(const GdbServer &other) = delete;

    virtual ~GdbServer();

    static void handleInterrupt(InterruptFrame &frame);

    static uint8_t computeSignal(uint8_t interrupt);

    static void initialize();

    static bool isInitialized();

};


#endif //HHUOS_GDBSERVER_H

//
// Created by krakowski on 08.06.18.
//

#ifndef __GdbServer_include__
#define __GdbServer_include__

#define BREAKPOINT() asm("   int $3");

#include <kernel/threads/ThreadState.h>

struct GdbRegisters {
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t pc;
    uint32_t ps;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;

    static GdbRegisters fromInterruptFrame(InterruptFrame &frame);
} __attribute__((packed));

class GdbServer {

public:

    GdbServer() = delete;

    GdbServer(const GdbServer &other) = delete;

    GdbServer &operator=(const GdbServer &other) = delete;

    virtual ~GdbServer();

    static void handleInterrupt(InterruptFrame &frame);

    static uint8_t computeSignal(uint8_t interrupt);

    static void initialize();

    static inline void synchronize() {
        BREAKPOINT();
    }

    static bool isInitialized();

};


#endif //HHUOS_GDBSERVER_H

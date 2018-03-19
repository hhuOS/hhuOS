#ifndef __DebugService_include__
#define __DebugService_include__

#include "kernel/KernelService.h"
#include "kernel/services/TimeService.h"

#include <cstdint>

#include <devices/input/Keyboard.h>
#include "kernel/interrupts/Pic.h"


class DebugService : public KernelService {

public:

    DebugService();

    void dumpMemory(uint32_t address, size_t lines);

    void dumpMemoryList();

    void printPic();

    static constexpr char* SERVICE_NAME = "DebugService";

private:

    char sanitize(char c);

    Keyboard *keyboard;

    TimeService *timeService;

    LinearFrameBuffer *lfb;

    Pic *pic;
};


#endif

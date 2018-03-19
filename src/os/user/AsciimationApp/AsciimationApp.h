/*****************************************************************************
 *                                                                           *
 *                          S T A R W A R S                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Star Wars Episode 1.                                     *
 *                                                                           *
 * Autor:           Filip Krakowski, 20.10.2017                              *
 *****************************************************************************/
#ifndef __AsciimationApp_include__
#define __AsciimationApp_include__

#include "kernel/threads/Thread.h"
#include "lib/File.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"
#include "kernel/services/TimeService.h"

/**
 * @author Filip Krakowski
 */
class AsciimationApp : public Thread {

private:

    AsciimationApp (const AsciimationApp &copy);

    uint32_t readDelay();

    void readLine(char *buf);

    void printFrame();

    const char *filePath = nullptr;

    File *file = nullptr;

    char* buffer = nullptr;

    LinearFrameBuffer *lfb;

    TimeService *timeService;

    uint16_t posX, posY;

public:

    explicit AsciimationApp (const char *path);

    void run() override;
};

#endif

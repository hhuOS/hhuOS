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
class AsciimationApp : public Thread, public Receiver {

private:

    uint32_t readDelay();

    void readLine(char *buf);

    void printFrame();

    char fileName[4096];

    uint64_t fileLength = 0;

    File *file = nullptr;

    char* buffer = nullptr;

    GraphicsService *graphicsService = nullptr;

    FileSystem *fileSystem = nullptr;

    TimeService *timeService = nullptr;

    EventBus *eventBus = nullptr;

    uint16_t posX, posY;

public:

    AsciimationApp();

    AsciimationApp(const AsciimationApp &copy) = delete;

    ~AsciimationApp() override = default;

    void onEvent(const Event &event) override;

    void run() override;
};

#endif

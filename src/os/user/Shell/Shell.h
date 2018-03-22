#ifndef __Shell_include__
#define __Shell_include__

#include <kernel/services/GraphicsService.h>
#include <kernel/services/EventBus.h>
#include <lib/file/Directory.h>
#include <kernel/services/StdStreamService.h>

class Command;

class Shell : public Thread, Receiver {

private:
    GraphicsService *graphicsService;
    EventBus *eventBus;
    OutputStream &stderr;

    Util::HashMap<String, Command*> commands;

    Directory *cwd = nullptr;

    char input[4096];

    void executeCommand();

public:

    /**
     * Constructor.
     */
    Shell();

    /**
     * Copy-constructor.
     */
    Shell(const Shell &copy) = delete;

    /**
     * Destructor.
     */
    ~Shell() override;

    Directory &getCurrentWorkingDirectory();

    void setCurrentWorkingDirectory(Directory *cwd);

    void run() override;

    void onEvent(const Event &event) override;
};

#endif
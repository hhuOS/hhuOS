#ifndef HHUOS_SCREENSHOTSERVICE_H
#define HHUOS_SCREENSHOTSERVICE_H

#include <kernel/KernelService.h>
#include <kernel/events/Receiver.h>

class ScreenshotService : public KernelService, public Receiver {

private:

    static uint32_t screenshotCounter;

public:

    ScreenshotService();

    ~ScreenshotService() override;

    void takeScreenshot();

    void onEvent(const Event &event) override;

    static constexpr const char* SERVICE_NAME = "ParallelService";
};

#endif

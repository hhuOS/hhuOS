#include <lib/String.h>
#include <lib/graphic/Screenshot.h>
#include <kernel/threads/WorkerThread.h>
#include <lib/file/FileStatus.h>
#include <kernel/events/input/KeyEvent.h>
#include "ScreenshotService.h"

uint32_t ScreenshotService::screenshotCounter = 0;

ScreenshotService::ScreenshotService() {
    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);
}

ScreenshotService::~ScreenshotService() {
    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
}

void ScreenshotService::takeScreenshot() {
    Screenshot screenshot;

    screenshot.take();

    if(!FileStatus::exists("/screenshots")) {
        Kernel::getService<FileSystem>()->createDirectory("/screenshots");
    }

    while(FileStatus::exists(String::format("/screenshots/screenshot%u.bmp", screenshotCounter))) {
        screenshotCounter++;
    }

    screenshot.saveToBmp(String::format("/screenshots/screenshot%u.bmp", screenshotCounter));
}

void ScreenshotService::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::PRINT) {
        takeScreenshot();
    }
}

//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_MOUSEAPP_H
#define HHUOS_MOUSEAPP_H


#include <apps/Application.h>
#include <kernel/services/EventBus.h>
#include <kernel/Kernel.h>
#include <kernel/events/input/MouseClickedEvent.h>
#include <kernel/events/input/MouseReleasedEvent.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/services/GraphicsService.h>

class MouseApp : public Thread, Receiver {

private:
    int32_t xPos;
    int32_t yPos;

    Color color;
    bool isRunning = true;

    LinearFrameBuffer *lfb;
    EventBus *eventBus = nullptr;

public:
    MouseApp(const MouseApp &copy) = delete;

    MouseApp() : Thread("MouseApp"), Receiver() {
        eventBus = Kernel::getService<EventBus>();

        lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
        xPos = lfb->getResX() / 2 - 25/2;
        yPos = lfb->getResY() / 2 - 25/2;
        color = Colors::WHITE;
    }

    ~MouseApp() override {
        eventBus->unsubscribe(*this, MouseMovedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseClickedEvent::TYPE);
        eventBus->unsubscribe(*this, MouseReleasedEvent::TYPE);
    }

    void onEvent(const Event &event) override;

    void run();

};


#endif //HHUOS_MOUSEAPP_H

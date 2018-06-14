#include <kernel/Kernel.h>
#include <kernel/events/input/KeyEvent.h>
#include <apps/LoopsAndSound/Loop.h>
#include <kernel/threads/Scheduler.h>
#include <apps/Application.h>
#include "LoopsAndSound.h"
#include "Sound.h"

LoopsAndSound::LoopsAndSound() {
    eventBus = Kernel::getService<EventBus>();
}

LoopsAndSound::~LoopsAndSound() {
    eventBus->unsubscribe(*this, KeyEvent::TYPE);
}

void LoopsAndSound::onEvent(const Event &event) {
    if(event.getType() == KeyEvent::TYPE) {
        auto &keyEvent = (KeyEvent&) event;

        if(keyEvent.getKey().scancode() == KeyEvent::ESCAPE) {
            isRunning = false;
        }
    }
}

void LoopsAndSound::run() {
    eventBus->subscribe(*this, KeyEvent::TYPE);

    Thread *thread1 = new Loop(1);
    Thread *thread2 = new Loop(2);
    Thread *thread3 = new Sound();

    thread1->start();
    thread2->start();
    thread3->start();

    while(isRunning);

    Scheduler::getInstance()->kill(*thread1);
    Scheduler::getInstance()->kill(*thread2);
    Scheduler::getInstance()->kill(*thread3);

    delete thread1;
    delete thread2;
    delete thread3;

    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}

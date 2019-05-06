#include "BootComponent.h"

BootComponent::BootComponent(const String &name, Util::Array<BootComponent*> dependencies, void (*function)()) :
        Thread(name, 0xff), waiting(true), finished(false), dependencies(dependencies), function(function) {

}

void BootComponent::run() {
    for(const auto &dependency : dependencies) {
        dependency->join();
    }

    waiting = false;

    function();

    finished = true;
}

bool BootComponent::isWaiting() {
    return waiting;
}

bool BootComponent::hasFinished() {
    return finished;
}

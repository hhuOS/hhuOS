#include <apps/Application.h>
#include "BootCoordinator.h"

BootCoordinator::BootCoordinator(Util::Array<BootComponent*> components, void (*onFinish)()) :
        Thread("BootCoordinator", 0xff), onFinish(onFinish) {
    for(const auto &component : components) {
        addComponent(component);
    }
}

void BootCoordinator::run() {
    for(const auto &component : components) {
        component->start();
    }

    for(const auto &component : components) {
        component->join();
    }

    onFinish();
}

void BootCoordinator::addComponent(BootComponent *component) {
    components.add(component);
}

Util::Array<BootComponent*> BootCoordinator::getComponents() {
    return components.toArray();
}

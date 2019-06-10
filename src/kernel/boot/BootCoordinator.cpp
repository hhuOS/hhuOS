#include "application/Application.h"
#include "lib/util/LinkedBlockingQueue.h"
#include "BootCoordinator.h"

BootCoordinator::BootCoordinator(Util::Array<BootComponent*> components, void (*onFinish)()) :
        Thread("BootCoordinator", 0xff), hasStarted(false), onFinish(onFinish) {
    for(const auto &component : components) {
        addComponent(component);
    }
}

void BootCoordinator::run() {
    if(hasStarted.get()) {
        return;
    }

    hasStarted.set(true);

    buildComponentLayers();

    for(const auto &layer : layeredComponents) {
        for (const auto &component : *layer) {
            component->start();
        }

        for (const auto &component : *layer) {
            component->join();
        }

        delete layer;
    }

    onFinish();
}

void BootCoordinator::addComponent(BootComponent *component) {
    if(!hasStarted.get()) {
        components.add(component);
    }
}

Util::Array<BootComponent*> BootCoordinator::getComponents() {
    return components.toArray();
}

void BootCoordinator::buildComponentLayers() {
    Util::LinkedBlockingQueue<BootComponent*> queue;

    for(uint32_t i = 0; !components.isEmpty(); i++) {
        layeredComponents.add(new Util::ArrayList<BootComponent*>());

        for (const auto &component : components) {
            if (!component->hasDependencies()) {
                queue.push(component);
                components.remove(component);
            }
        }

        for (const auto &queueComponent : queue) {
            for (const auto &component : components) {
                component->removeDependency(queueComponent);
            }

            queue.remove(queueComponent);
            layeredComponents.get(i)->add(queueComponent);
        }
    }

    // Refill components list in correct order for printing them onto the bootscreen
    for(const auto &layer : layeredComponents) {
        for (const auto &component : *layer) {
            components.add(component);
        }
    }
}

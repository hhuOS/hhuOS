#ifndef HHUOS_BOOTSEQUENCE_H
#define HHUOS_BOOTSEQUENCE_H

#include <kernel/threads/Thread.h>
#include <lib/lock/Spinlock.h>
#include <lib/util/ArrayList.h>
#include "BootComponent.h"

class BootCoordinator : public Thread {

public:

    explicit BootCoordinator(Util::Array<BootComponent*> components, void (*onFinish)());

    BootCoordinator(const BootCoordinator &copy) = delete;

    BootCoordinator& operator=(const BootCoordinator &other) = delete;

    ~BootCoordinator() override = default;

    void run() override;

    void addComponent(BootComponent* component);

    Util::Array<BootComponent*> getComponents();

private:

    Util::ArrayList<BootComponent*> components;

    void (*onFinish)();

};

#endif

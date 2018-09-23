#include <kernel/Kernel.h>
#include "FloppyMotorControlThread.h"

FloppyMotorControlThread::FloppyMotorControlThread(FloppyDevice &device) : device(device), timeout(FloppyController::FLOPPY_TIMEOUT), isRunning(true) {
    timeService = Kernel::getService<TimeService>();
}

void FloppyMotorControlThread::run() {
    while(isRunning) {
        if(device.motorState == FloppyController::FLOPPY_MOTOR_WAIT) {
            timeService->msleep(500);
            timeout -= 500;

            if(timeout <= 0) {
                device.controller.killMotor(device);

                timeout = FloppyController::FLOPPY_TIMEOUT;
            }
        }

        yield();
    }
}

void FloppyMotorControlThread::stop() {
    isRunning = false;
}

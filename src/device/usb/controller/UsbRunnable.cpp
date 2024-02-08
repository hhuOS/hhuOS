#include "UsbRunnable.h"
extern "C"{
#include "UsbController.h"
}

UsbRunnable::UsbRunnable(UsbController* c) : controller(c){}

void UsbRunnable::run(){
    controller->runnable_function(controller);
}
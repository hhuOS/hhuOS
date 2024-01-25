#include "UsbRunnable.h"
#include "UsbController.h"

UsbRunnable::UsbRunnable(void* c) : controller(c){}

void UsbRunnable::run(){
    ((UsbController*)controller)->poll((UsbController*)controller);
}
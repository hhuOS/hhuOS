#include "Hello.h"
#include "lib/libc/printf.h"

extern "C" {
    Module *moduleProvider();
}

Module *moduleProvider() {

    return new Hello();
}

int32_t Hello::initialize() {

    printf("Hello hhuOS!\n");

    return 0;
}

int32_t Hello::finalize() {

    printf("Bye hhuOS!\n");

    return 0;
}

String Hello::getName() {

    return "hello";
}

Util::Array<String> Hello::getDependencies() {

    return Util::Array<String>(0);
}
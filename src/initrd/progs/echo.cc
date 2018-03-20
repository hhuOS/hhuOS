#include "../../os/lib/libc/printf.h"

#ifdef __cplusplus 
extern "C" {
#endif

int main(int argc, char *argv[]) {
    if (argc != 1) {
        return -1;
    }


    printf(argv[0]);

    return 0;
}

void _start() {
    int argc;
    char **argv;

    asm ("mov 8(%%ebp)  , %0" : "=r"(argc));
    asm ("mov 12(%%ebp) , %0" : "=r"(argv));

    main(argc, argv);
}

#ifdef __cplusplus
}
#endif

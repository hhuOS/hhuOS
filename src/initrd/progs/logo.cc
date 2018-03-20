#include "printf.h"

#ifdef __cplusplus 
extern "C" {
#endif

int main(int argc, char *argv[]) {

    printf("\n");
    printf("  _     _            ____   _____ \n");
    printf(" | |   | |          / __ \\ / ____|\n");
    printf(" | |__ | |__  _   _| |  | | (___  \n");
    printf(" | '_ \\| '_ \\| | | | |  | |\\___ \\ \n");
    printf(" | | | | | | | |_| | |__| |____) |\n");
    printf(" |_| |_|_| |_|\\__,_|\\____/|_____/ \n");
    printf("\n");

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

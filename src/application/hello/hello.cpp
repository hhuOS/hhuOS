#include <cstdint>

int32_t main(int32_t argc, char *argv[], void *envp) {
    int *test1 = new int[8192];
    delete[] test1;
    return 1797;
}

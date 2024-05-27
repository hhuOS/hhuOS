

typedef unsigned long int size_t;

//memory management
void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void *ptr);



//program execution
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
void abort();
void exit(int exit_code);
int atexit(void (*func)(void));
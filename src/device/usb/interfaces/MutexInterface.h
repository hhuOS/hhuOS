#ifndef MutexInterface__include
#define MutexInterface__include

typedef void* Mutex_P;

struct Mutex_C{
    void (*new_mutex)(struct Mutex_C* mutex_c);

    void (*acquire_c)(struct Mutex_C* mutex_c);
    int (*try_acquire_c)(struct Mutex_C* mutex_c);
    void (*release_c)(struct Mutex_C* mutex_c);
    int (*isLocked_c)(struct Mutex_C* mutex_c);

    Mutex_P mutex_pointer;
};

typedef struct Mutex_C Mutex_C;

#ifdef __cplusplus
extern "C"{
#endif

void new_mutex(Mutex_C* mutex_c);
void acquire_c(Mutex_C* mutex_c);
int try_acquire_c(Mutex_C* mutex_c);
void release_c(Mutex_C* mutex_c);
int isLocked_c(Mutex_C* mutex_c);

#ifdef __cplusplus
}
#endif

#endif

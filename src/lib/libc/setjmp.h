#ifndef LIBC_SETJMP
#define LIBC_SETJMP

#define setjmp(env) _setjmp(&env)
#define longjmp(env, status) _longjmp(&env, status)

typedef struct{
	unsigned long esp;
	unsigned long ebp;
	unsigned long eip;
} jmp_buf;


#ifdef __cplusplus
extern "C" {
#endif

int _setjmp(jmp_buf * env);
void _longjmp(jmp_buf * env, int status);

#ifdef __cplusplus
}
#endif

#endif
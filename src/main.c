#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])	
{	
	while(1) {
		/* do nothing */
	}
    return 0;
}


static inline void init_bss(void)
{
    extern unsigned int _bss;
    extern unsigned int _ebss;
    unsigned int *dst;

    dst = &_bss;
    while(dst < &_ebss)
        *dst++ = 0;
}

	void _init_bsp(int core_id, int number_of_cores)
	{
	extern int main(int argc, char *argv[]);
	extern void __libc_init_array(void);
	extern void __libc_fini_array(void);
	int ret = 0;
	if(core_id == 0)
	{
		/* Initialize bss data to 0 */
		init_bss();
		/* Register finalization function */
		atexit(__libc_fini_array);
		/* Init libc array for C++ */
		__libc_init_array();
		/* more stuff to init.. */
		/* call into main */
		ret = main(0, NULL);
	} else 
	{
		/* other core entry.. */
		ret = -1;
	}

	exit(ret);
	}


uintptr_t handle_syscall(uintptr_t cause, uintptr_t epc, uintptr_t regs[32], uintptr_t fregs[32])
{
	/* return function pointer to handler function, see https://github.com/kendryte/kendryte-standalone-sdk/blob/06a2ea71f250e91d66fa156ff82ae1f5b9fc6e56/lib/bsp/syscalls.c#L735 */
	return (uintptr_t) NULL;
}


uintptr_t handle_irq(uintptr_t cause, uintptr_t epc, uintptr_t regs[32], uintptr_t fregs[32])
{
	/* see https://github.com/kendryte/kendryte-standalone-sdk/blob/develop/lib/bsp/interrupt.c */
	return (uintptr_t) NULL;
}

/* needed for the C level APIs in atexit() */
#include <sys/lock.h>
typedef long _lock_t;

#define atomic_swap(ptr, swp) __sync_lock_test_and_set(ptr, swp)

/* Defination of memory barrier macro */
#define mb()                          \
    {                                 \
        asm volatile("fence" ::       \
                         : "memory"); \
    }


static inline long lock_trylock(_lock_t *lock)
{
    long res = atomic_swap(lock, -1);
    /* Use memory barrier to keep coherency */
    mb();
    return res;
}

static inline void lock_lock(_lock_t *lock)
{
    while(lock_trylock(lock))
        ;
}

static inline void lock_unlock(_lock_t *lock)
{
    /* Use memory barrier to keep coherency */
    mb();
    atomic_swap(lock, 0);
    asm volatile("nop");
}

void _lock_acquire_recursive(_lock_t *lock)
{
	//supposed to be a recursive lock but changed it for smaller implementation.
	//might not work.
    lock_lock(lock);
}


void _lock_release_recursive(_lock_t *lock)
{
//    reculock_unlock(lock);
	lock_unlock(lock);
}
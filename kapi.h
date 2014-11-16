#include <pthread.h>

#define CYG_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define CYG_COND_INITIALIZER PTHREAD_COND_INITIALIZER

typedef int res_t;
typedef pthread_mutex_t cyg_mutex_t;
typedef pthread_cond_t cyg_cond_t;
typedef pthread_t cyg_thread_t;
typedef pthread_attr_t cyg_thread_attr_t;

int cyg_thread_create(cyg_thread_t * thread, const cyg_thread_attr_t * attr, void *(*start_routine) (void *), void *arg)
{
    return pthread_create(thread, attr, start_routine, arg);
}

int cyg_thread_join(cyg_thread_t thread, void **retval)
{
    return pthread_join(thread, retval);
}

cyg_thread_t cyg_thread_self()
{
    return pthread_self();
}

int cyg_mutex_lock(cyg_mutex_t * res_lock)
{
    return pthread_mutex_lock(res_lock);
}

int cyg_mutex_unlock(cyg_mutex_t * res_lock)
{
    return pthread_mutex_unlock(res_lock);
}

int cyg_cond_wait(cyg_cond_t * res_wait, cyg_mutex_t * res_lock)
{
    return pthread_cond_wait(res_wait, res_lock);
}

int cyg_cond_signal(cyg_cond_t * res_wait)
{
    return pthread_cond_signal(res_wait);
}

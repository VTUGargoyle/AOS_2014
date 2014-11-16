/* Just a simulation, setting up ECOS looks like p.i.t.a ! */

#include <stdio.h>
#include <stdlib.h>
#include "kapi.h"

#define RES_MAX 10
#define PROC_MAX 8

typedef int res_t;
cyg_mutex_t res_lock = CYG_MUTEX_INITIALIZER;
cyg_cond_t res_wait = CYG_COND_INITIALIZER;

res_t res_pool[PROC_MAX];
int res_count = PROC_MAX;

void res_init()
{
    int i;
    for (i = 0; i < PROC_MAX; i++) // fill resource pool
        res_pool[i] = i + 1;
}

res_t res_allocate()
{
    res_t res;
    cyg_mutex_lock(&res_lock);
    
    while (res_count == 0)
        cyg_cond_wait(&res_wait, &res_lock);
    res_count--;
    res = res_pool[res_count];
    printf("TID %lu using %d, free resources: %d\n", cyg_thread_self(), res, res_count);
    cyg_cond_signal(&res_wait);
    
    cyg_mutex_unlock(&res_lock);
    return res;
}

void res_free(res_t res)
{
    cyg_mutex_lock(&res_lock);
    res_pool[res_count] = res;
    res_count++;
    printf("TID %lu freeing %d, free resources: %d\n", cyg_thread_self(), res, res_count);
    cyg_mutex_unlock(&res_lock);
}

void *run()
{
    res_t res;
    res_init();
    res = res_allocate();
    sleep(1);
    res_free(res);
    return 0;
}

int main()
{
    int i;
    cyg_thread_t thread[PROC_MAX];
    for (i = 0; i < PROC_MAX; i++)
        cyg_thread_create(&thread[i], NULL, &run, NULL);
    for (i = 0; i < PROC_MAX; i++)
        cyg_thread_join(thread[i], NULL);
    return 0;
}

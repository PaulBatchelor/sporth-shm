/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include "shared_mem.h"

static int sp_shm_get(shared_mem *sm, key_t key) 
{
    sm->key = key;

    if ((sm->shmid = shmget(sm->key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return PLUMBER_NOTOK;
    }
    if ((sm->shm = shmat(sm->shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return PLUMBER_NOTOK;
    }
}

static int sporth_shm_client(plumber_data *pd, sporth_stack *stack, void **ud)
{
    shared_mem *sm;
    char *str;
    SPFLOAT key;
    float *val;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "sf") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            sm = malloc(sizeof(shared_mem));
            key = sporth_stack_pop_float(stack);
            str = sporth_stack_pop_string(stack);
            sp_shm_get(sm, key);

            plumber_ftmap_delete(pd, 0);
            val = sm->shm;
            plumber_set_var(pd, str, val);
            plumber_ftmap_delete(pd, 1);
            *ud = sm;
            break;
        case PLUMBER_INIT:
            key = sporth_stack_pop_float(stack);
            str = sporth_stack_pop_string(stack);
            break;
        case PLUMBER_COMPUTE:
            sporth_stack_pop_float(stack);
            break;
        case PLUMBER_DESTROY:
            sm = *ud;
            free(sm);
            break;
    }

    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_shm_client;
}

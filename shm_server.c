#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>

#include "shared_mem.h"


static int sp_shm_set(shared_mem *sm, key_t key, SPFLOAT *val) 
{
    sm->key = key;
    if ((sm->shmid = shmget(sm->key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
        return PLUMBER_NOTOK;
    }
    if ((sm->shm = shmat(sm->shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return PLUMBER_NOTOK;
    }
    sm->s = sm->shm;
    int i;
    //unsigned char *mem = (unsigned char *)&sm->val;
    unsigned char *mem = (unsigned char *)val;
  
    for(i = 0; i < sizeof(SPFLOAT); i++) {
        *sm->s++ = mem[i];
    }
    return PLUMBER_OK;
}

static int sporth_shm_server(plumber_data *pd, sporth_stack *stack, void **ud)
{
    shared_mem *sm;
    const char *str;
    SPFLOAT key;
    SPFLOAT *val;
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
            
            sp_shm_set(sm, key, &sm->val);
            sm->val = 1234;
            val = (SPFLOAT *)sm->shm;
            plumber_ftmap_delete(pd, 0);
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
/*
int main()
{
    shared_mem sm;

    SPFLOAT val = 1234;
    sp_shm_set(&sm, 5678, &val);
    //sm.val = 3.14159;
    //sm.key = 5678;
    //if ((sm.shmid = shmget(sm.key, SHMSZ, IPC_CREAT | 0666)) < 0) {
    //    perror("shmget");
    //    exit(1);
    //}
    //if ((sm.shm = shmat(sm.shmid, NULL, 0)) == (char *) -1) {
    //    perror("shmat");
    //    exit(1);
    //}
    //sm.s = sm.shm;
    //int i;
    //unsigned char *mem = (unsigned char *)&sm.val;
  
    //for(i = 0; i < sizeof(sp_ftbl); i++) {
    //    *sm.s++ = mem[i];
    //}

    while (*sm.shm != '*') sleep(1);

    exit(0);
}
*/
plumber_dyn_func sporth_return_ugen()
{
    return sporth_shm_server;
}

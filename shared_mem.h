typedef struct {
    char c;
    int shmid;
    key_t key;
    char *shm, *s;
    SPFLOAT val;
} shared_mem;

#define SHMSZ     27


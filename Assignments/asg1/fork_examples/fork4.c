#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 3

int main() {
    int shmid = shmget(IPC_PRIVATE, N * sizeof(int), IPC_CREAT | 0666);
    int *shm_ptr = (int *)shmat(shmid, NULL, 0);

    for (int i = 0; i < N; ++i) {
        if (fork() == 0) {
            shm_ptr[i] = getpid();
            shmdt(shm_ptr);
            exit(0);
        }
    }

    // Parent waits for 3 children
    for (int i = 0; i < N; ++i){
        wait(NULL);
    }
    
    // Parent prints all results
    printf("Shared memory contents:\n");
    for (int i = 0; i < N; ++i) {
        printf("%d ", shm_ptr[i]);
    }
    printf("\n");

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

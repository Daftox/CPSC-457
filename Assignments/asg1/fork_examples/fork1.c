#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t pid;

    /* Try to fork a new process */
    pid = fork();

    if (pid < 0)
    {
        /* fork failed */
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        /* This block runs in the child process */
        printf("Child:  My PID is %d\n", getpid());
        printf("Child:  fork() returned %d to me\n", pid);
    }
    else
    {
        /* This block runs in the parent process */
        printf("Parent: My PID is %d\n", getpid());
        printf("Parent: fork() returned %d (the child's PID) to me\n", pid);
    }

    /* Both parent and child continue from here */
    printf("Process %d: I reach the end of main()\n", getpid());
    return 0;
}

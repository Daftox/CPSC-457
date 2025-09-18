#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 3

int main() {
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        if (pid == 0) {
            // Each child does some work
            printf("Child %d (PID %d): Doing some work\n", i, getpid());

            sleep(2); // Simulate work by sleeping for 1 second

            // _exit: terminate the child process immediately 
            // _exit code range is unsigned 8 bits (0-255)
            _exit(0);
        }
        // Parent continues to next loop
    }

    // Parent waits for all children
    for (int i = 0; i < N; i++) {
        // wait: parent should wait for any child to finish
        wait(NULL);
    }
    printf("Parent: All children finished.\n");
    return 0;
}

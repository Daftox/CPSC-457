#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        printf("Child running... %d\n", getpid());
        // The valid exit codes are in the range: 0-255 
        // If you pass a larger or negative number, it gets truncated modulo 256
        // exit(300); → OS sees 300 % 256 = 44
        // exit(-1); → OS sees 255
        _exit(42);   // child exits with code 42
    } 
    else if (pid > 0) {
        // Parent process
        int status;
        pid_t pid_child = wait(&status);  // wait for child to finish

        printf("Parent PID is %d.\n", getpid());
        printf("Parent: My child's PID is %d\n", pid);
        printf("Parent: Child with PID %d has terminated.\n", pid_child);
        printf("Parent: Child's termination status is %s\n", WIFEXITED(status) ? "normal" : "abnormal");
        printf("Parent: Child's exit status is %d\n", WEXITSTATUS(status));

        if (WIFEXITED(status)) {
            printf("Child exited with code %d\n", WEXITSTATUS(status));
        } else {
            printf("Child did not exit normally\n");
        }
    } 
    else {
        perror("fork failed");
        return 1;
    }

    return 0;
}

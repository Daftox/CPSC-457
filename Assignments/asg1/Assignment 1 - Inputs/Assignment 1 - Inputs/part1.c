#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ROWS 100
#define COLUMNS 1000

int main() {

    int fd[2];   // fd[0] = read end, fd[1] = write end
    if (pipe(fd) == -1) {
        perror("pipe failed");
        _exit(1);
    }

    char line[2*COLUMNS+1]; //little extra room to handle EOL

    // PARENT - INIT ------------------------------------
    for (int i = 0; i < ROWS; i++) {

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        }

        // CHILDS ----------------------------------------
        if (pid == 0) {

            close(fd[0]); // close unused read end
            //printf("Child %d (PID %d): Searching row\n", i, getpid(), i);

            char *p = line;    // pointer to start of line
            int col = 0;

            while (col < COLUMNS && *p != '\0' && *p != '\n') {
                if (*p == '1') {
                    if (write(fd[1], &col, sizeof col) == -1) {
                        perror("write pipe failed");
                        _exit(1);
                    }
                    close(fd[1]);
                    _exit(0);  // treasure found
                }

                if (*p != '0'){
                    printf("unexpected value %d in the provided text file", *p);
                    _exit(1);
                }
                col++;
                p+=2;  // move pointer forward, ignoring the space
            }

            close(fd[1]); // close unused write end
            _exit(1);
        }// ----------------------------------------------
    }

    // PARENT - CLOSE ------------------------------------
    close(fd[1]); // close unused write end

    int treasure_col;
    if (read(fd[0], &treasure_col, sizeof(treasure_col)) == -1) {
        perror("read pipe failed");
        _exit(1);
    }

    for (int i = 0; i < ROWS; i++) {
        int status;
        pid_t pid_child = wait(&status);
        if (WEXITSTATUS(status) == 0){
            printf("Parent: The treasure was found by child with PID %d at row %d, column %d\n", pid_child, i, treasure_col);
            close(fd[0]); // close unused read end
            _exit(0);
            return 0;
        }
    }

    close(fd[0]); // close unused read end

    printf("Parent: No treasure was found in the matrix\n");

    _exit(0);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ROWS 100
#define COLUMNS 1000

int main()
{
    char rows[ROWS][2 * COLUMNS + 1]; // little extra room to handle EOL

    // PARENT - INIT ------------------------------------
    for (int i = 0; i < ROWS; i++)
    {

        // save each child's line in the rows array
        if (fgets(rows[i], sizeof(rows[i]), stdin) == NULL)
        {
            break;
        }

        // checks the length of the read line
        if (rows[i][2 * COLUMNS - 1] != '\n')
        {
            printf("Critical invalid input error at line %d: wrong number of characters.\n", i);
            return 0;
        }

        // create a child
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            return 1;
        }

        // CHILDS ----------------------------------------
        if (pid == 0)
        {
            /* note that there are contradictory expectations regarding this print
            between the README file in Inputs folder and the assignment description
            The line is printed to respect the expectation of "Assignment 1 - Fork.pdf" file
            */
            printf("Child %d (PID %d): Searching row %d\n", i, getpid(), i);
            char *p = rows[i]; // pointer to start of the line
            int col = 0;

            while (col < COLUMNS && *p != '\0' && *p != '\n')
            {
                if (*p == '1')
                {
                    _exit(i); // treasure found exit code 0-99
                }
                else if (*p != '0')
                {
                    _exit(100 + i); // invalid line exit code 100-199
                }
                col++;
                p += 2; // move pointer forward twice, ignoring the spaces (or any other char) in the odd indexes
            }

            _exit(200); // no treasure found exit code 200
        } // ----------------------------------------------
    }

    // PARENT - CLOSE ------------------------------------

    int treasure_row = -1;
    int more_than_one_treasure = 0;
    pid_t pid_child_treasure;
    int invalid_input_line = -1;

    for (int i = 0; i < ROWS; i++)
    {
        int status;
        pid_t pid_child = wait(&status);

        if (WEXITSTATUS(status) < 100 && !more_than_one_treasure)
        {
            if (treasure_row != -1)
            {
                more_than_one_treasure = 1; // will inform the user that there is more thane one treasure
            }
            else
            {
                pid_child_treasure = pid_child;
                treasure_row = WEXITSTATUS(status); // only first treasure will be kept
            }
        }
        else if (WEXITSTATUS(status) >= 100 && WEXITSTATUS(status) < 200 && invalid_input_line == -1)
        {
            invalid_input_line = WEXITSTATUS(status) - 100; // the first invalid line is found by removing the 100 offset in its exit code
        }
    }

    if (invalid_input_line > -1)
    {
        // an invalid input (value other than "0" or "1" in the even indexes) will be considered as a "0"
        // this printf simply informs the user that there is an invalid input in the text file
        printf("Invalid input at line %d in text file: should be filled with 0 or 1 only\n", invalid_input_line);
    }

    if (treasure_row >= 0 && treasure_row < 100)
    {

        char *p = rows[treasure_row]; // pointer to start of the line containing the treasure
        int treasure_col = 0;

        while (treasure_col < COLUMNS && *p != '\0' && *p != '\n')
        {
            if (*p == '1')
            {
                printf("Parent: The treasure was found by child with PID %d at row %d, column %d\n", pid_child_treasure, treasure_row, treasure_col);
                if (more_than_one_treasure)
                {
                    printf("Invalid input text file: there is more than one treasure\n");
                }
                return 0;
            }
            treasure_col++;
            p += 2; // move pointer forward twice, ignoring the spaces (or any other char) in the odd indexes
        }
    }

    printf("Parent: No treasure was found in the matrix\n");

    return 0;
}

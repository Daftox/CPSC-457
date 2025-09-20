#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <math.h>

// Provided primality testing function
int is_prime(int num)
{
    if (num < 2)
        return 0;
    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    // ___________________________________________________________
    // PARENT'S INITIAL PREPARATIONS
    // -----------------------
    // command line arguments
    if (argc != 4)
    {
        printf("Number of arguments is invalid\n");
        return 1;
    }
    int lower_bound = atoi(argv[1]);
    int upper_bound = atoi(argv[2]);
    int children = atoi(argv[3]);
    if (lower_bound > upper_bound || children <= 0)
    {
        printf("Invalid arguments.\n");
        return 1;
    }

    // set up variables: amount of numbers, amount of children and range per child
    int numbers = upper_bound - lower_bound + 1;
    children = children > numbers ? numbers : children; // truncate number of children if necessary
    int range_size = numbers / children;

    // set up variables: memory per child, for last child, and shared memory
    int max_primes_per_child = range_size <= 1 ? 1 : range_size / 2;                             // divided by 2 because even numbers can be ignored
    int max_primes_last_child = (upper_bound - (lower_bound + (children - 1) * range_size)) / 2; // last child will test all remaining numbers
    max_primes_last_child = max_primes_last_child <= 0 ? 1 : max_primes_last_child;
    int shm_size = ((children - 1) * max_primes_per_child + max_primes_last_child) * sizeof(int); // compute the required size of shared memory

    // uncomment below to print the previously defined variables
    /* printf("Numbers %d, Children %d, Range Size %d\n", numbers, children, range_size);
       printf("Max primes per child %d, Shm size %d\n", max_primes_per_child, shm_size);*/

    // shared memory allocation
    int shmid = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        return 1;
    }

    int *shm_ptr = (int *)shmat(shmid, NULL, 0);
    if (shm_ptr == (int *)-1)
    {
        perror("shmat failed");
        return 1;
    }

    // initialize shared memory with -1
    for (int i = 0; i < shm_size / sizeof(int); i++)
    {
        shm_ptr[i] = -1;
    }

    // creation of childs processes
    for (int i = 0; i < children; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            return 1;
        }

        // ___________________________________________________________
        // CHILDS
        // -----------------------
        if (pid == 0)
        {
            // set up range of numbers to be tested
            int start_range = lower_bound + range_size * i;
            int end_range = start_range + range_size - 1;

            // the last child handles the whole remaining range
            if (i == children - 1)
            {
                end_range = upper_bound;
            }

            printf("Child PID %d checking range [%d, %d]\n", getpid(), start_range, end_range);

            int *child_shm_ptr = shm_ptr + (i * max_primes_per_child); // current child's offset in shared memory

            // primality testing
            for (int j = start_range; j <= end_range; j++)
            {
                if (is_prime(j))
                {
                    *child_shm_ptr = j;
                    child_shm_ptr++;
                }
            }

            shmdt(shm_ptr);
            _exit(0);
        }
    }

    // ___________________________________________________________
    // PARENT'S FINAL READ AND CLEANUP
    // -----------------------
    // wait for all children to finish their process
    int status;
    for (int i = 0; i < children; i++)
    {
        wait(&status);
    }

    printf("\nParent: All children finished. Primes found:\n");

    // print all the primes that were found by children
    for (int i = 0; i < children; i++)
    {
        int *child_shm_ptr = shm_ptr + (i * (max_primes_per_child)); // current child's offset in shared memory

        if (i == children - 1)
        {
            max_primes_per_child = max_primes_last_child;
        }

        int j = 0;
        while (*child_shm_ptr > 0 && j++ < max_primes_per_child) // no more prime number were found by this child if the read value is "-1"
        {
            printf("%d ", *child_shm_ptr++);
        }
    }

    printf("\n");

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
#include <stdio.h>
int main()
{
    int x;
    printf("Enter a number: ");
    scanf("%d", &x);                // Reads input into x
    printf("You entered: %d\n", x); // Prints x
    char name[50];                  // a character array to store a string with null terminator
    scanf("%s", name);              // Read a string from input
    printf("Hello, %s!\n", name);   // Print the string
    char buf[100];
    printf("Enter a line: ");
    fgets(buf, 100, stdin); // Reads a line of input (including spaces)
    printf("You entered: %s", buf);
    FILE *fp = fopen("output.txt", "w"); // Opens file for writing
    if (fp)
    {
        fprintf(fp, "Number: %d\n", x); // Writes to file
        fclose(fp);
    }
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <iostream>

int main()
{
    printf("Hello\n");
    /* create & run child process - a duplicate of parent */
    fork();
    /* both parent and child will execute the next line */
    printf("world.\n");
}
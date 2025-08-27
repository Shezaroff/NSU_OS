#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int arc, char** argv) {
    printf("PID: %d\n", getpid());
    sleep(1);
    execv("task2_a", argv);
    printf("Hello World!");
    perror("error execvp");
    printf("ERROR\n");
    return EXIT_SUCCESS;
}
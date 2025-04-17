#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("PID: %d\n", getpid());
    sleep(1);
    execvp("./task2_a", NULL);
    perror("error execvp");
    printf("ERROR\n");
    return EXIT_SUCCESS;
}

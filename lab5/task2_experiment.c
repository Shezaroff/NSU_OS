#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int global_var = 123;

int main(int argc, char** argv) {
    printf("PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        printf("child PID: %d\n", getpid());
        sleep(10);

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork error");
            return EXIT_FAILURE;
        } else if (pid2 == 0) {
            printf("grandchild PID: %d\n", getpid());
            sleep(300);
        } else {
            exit(2);
        }
    } else {
        sleep(300);
    }

    return EXIT_SUCCESS;
}

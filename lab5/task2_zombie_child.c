#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int global_var = 123;

int main(int argc, char** argv) {
    int local_var = 321;

    printf("global_var: %p (%d)\n", &global_var, global_var);
    printf("local_var: %p (%d)\n", &local_var, local_var);

    printf("PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        printf("PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        printf("child global_var: %p (%d)\n", &global_var, global_var);
        printf("child local_var: %p (%d)\n", &local_var, local_var);  
    
        global_var = 789;
        local_var = 987;
        sleep(10);

        printf("child modified global_var: %p (%d)\n", &global_var, global_var);
        printf("child modified local_var: %p (%d)\n", &local_var, local_var);
        exit(5);
    } else {
        printf("parent global_var: %p (%d)\n", &global_var, global_var);
        printf("parent local_var: %p (%d)\n", &local_var, local_var);
        sleep(300);
    }

    return EXIT_SUCCESS;
}

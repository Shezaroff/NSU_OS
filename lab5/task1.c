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

        printf("child modified global_var: %p (%d)\n", &global_var, global_var);
        printf("child modified local_var: %p (%d)\n", &local_var, local_var);
        exit(5);
    } else {
        printf("parent global_var: %p (%d)\n", &global_var, global_var);
        printf("parent local_var: %p (%d)\n", &local_var, local_var);
        sleep(30);
        
        int status;
        pid_t wpid = wait(&status);

        if (wpid == -1) {
            perror("wait error");
            return 1;
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("exit code: %d\n", exit_code);
        } else {
            printf("unknown reason\n");
        }
    }

    return EXIT_SUCCESS;
}

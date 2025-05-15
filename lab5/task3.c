#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#define STACK_SIZE (1024 * 16) 
#define STACK_PATH  "stack.bin"

void recursion(int iter) {
    char str[] = "hello world";
    printf("next iter\n");
    sleep(15);

    if (iter > 0) {
        recursion(iter - 1);
    }
}

int entry_point(void* arg) {
    printf("%p\n", (void*)recursion);
    printf("PID = %d, PPID = %d\n", getpid(), getppid());
    recursion(10);
    return 0;
}

int main() {
    int fd = open(STACK_PATH, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, STACK_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void* stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    void* stack_top = (char*)stack + STACK_SIZE;

    pid_t pid = clone(entry_point, stack_top, SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    int status;
    pid_t wpid = wait(&status);

    if (wpid == -1) {
        perror("wait error");
        return 1;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("child exit code: %d\n", exit_code);
    } else {
        printf("unknown reason\n");
    }

    munmap(stack, STACK_SIZE);
    close(fd);

    return 0;
}

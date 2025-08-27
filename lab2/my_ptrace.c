#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

//  /usr/include/x86_64-linux-gnu/asm/unistd_64.h

int index_sys(int indx) {
    return MAX_LINE_LENGTH * indx;
}

void child_pg(const char *program_name) {
    printf("program for trace '%s'\n", program_name);
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        perror("ptrace");
        exit(EXIT_FAILURE);
    }
    if (execl(program_name, program_name, NULL) == -1) {
        perror("execl");
        exit(EXIT_FAILURE);
    }
}

void parse_file(char* path, char* syscalls, int length) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    file = fopen(path, "r");

    if (file == NULL) {
        perror("Ошибка при открытии файла");
        free(syscalls);
        exit(EXIT_FAILURE);
    }

    char name[MAX_LINE_LENGTH];
    int num;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp("#define __NR_", line, 13) == 0) {
            if (sscanf(line, "#define __NR_%127s %d", name, &num) != 2) {
                printf("Ошибка при считывании файла");
                free(syscalls);
                exit(EXIT_FAILURE);
            }
            strcpy(syscalls + index_sys(num), name);
        }
    }

    fclose(file);
}

int current_pg(pid_t child_pid, char* syscalls, int length) {
    int wait_status;
    unsigned long long syscall;
    if (wait(&wait_status) == -1) {
        perror("wait");
        free(syscalls);
        return EXIT_FAILURE;
    }
    while (WIFSTOPPED(wait_status)) {
        struct user_regs_struct regs;
        if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) == -1) {
            perror("ptrace");
            free(syscalls);
            return EXIT_FAILURE;
        }
        if (wait(&wait_status) == -1) {
            perror("wait");
            return EXIT_FAILURE;
        }
        if (WIFEXITED(wait_status)) {
            break;
        }
        if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) == -1) {
            perror("ptrace");
            return EXIT_FAILURE;
        }
        syscall = regs.orig_rax;
        printf("+ %s", syscalls + index_sys(syscall));
        if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) == -1) {
            perror("ptrace");
            return EXIT_FAILURE;
        }
        if (wait(&wait_status) == -1) {
            perror("wait");
            return EXIT_FAILURE;
        }
        if (WIFEXITED(wait_status)) {
            break;
        }

        // Чтение результата после syscall
        if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) == -1) {
            perror("ptrace");
            return EXIT_FAILURE;
        }

        unsigned long retval = regs.rax;

        if ((long)retval < 0) {
            printf(", error: %ld\n", (long)retval);
        } else {
            printf(" = 0x%lx\n", retval);
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "./program program_for_trace\n");
        return 1;
    }
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (child_pid == 0) {
        child_pg(argv[1]);
    } else {
        char* syscalls = malloc(sizeof(char) * 462 * 128);
        if (syscalls == NULL) {
            printf("malloc error\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < 462; i++) {
            strcpy(syscalls + index_sys(i), "?");
        }
        parse_file("/usr/include/x86_64-linux-gnu/asm/unistd_64.h", syscalls, 462);
    
        int return_value = current_pg(child_pid, syscalls, 462);
        free(syscalls);
        return return_value;
    }
    return EXIT_SUCCESS;
}
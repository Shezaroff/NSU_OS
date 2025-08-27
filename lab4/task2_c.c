#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>

void stack_f(int a) {
    int array[1024 * 1024];
    printf("%d kb\n", a);
    sleep(1);
    stack_f(a + 4);
}

void heap_f() {
    int i;
    int num_iterations = 100; 
    char **buffers = malloc(num_iterations * sizeof(char*));
    for(i = 0; i < num_iterations; i++) {
        buffers[i] = NULL;
    }

    if (buffers == NULL) {
        perror("malloc failed");
        return;
    }

    int size = 0;
    for (i = 0; i < num_iterations; i++) {
        buffers[i] = malloc(1024 * 1024 * sizeof(char));  
        size += 12;
        printf("%d Mb\n", size);

        if (buffers[i] == NULL) {
            perror("malloc failed");
            for (int j = 0; j < i; j++) {
                if (buffers[j] != NULL) {
                    free(buffers[j]);
                }
            }
            free(buffers);
            return;
        }
        sleep(1);
    }

    printf("Absolute free\n");

    for (i = 0; i < num_iterations; i++) {
        free(buffers[i]);
    }

    free(buffers);
}

void sigsegv_handler(int signum) {
    write(2, "SIGSEGV signal\n", 15);
    exit(EXIT_FAILURE);
}

void write_mmap_f() {
    

    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t size = 10 * page_size;

    sleep(5);
    char *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return;
    }
    printf("New region: %p\n", ptr);

    strcpy(ptr, "USELESS HELLO WORLD: GRAND FINALE\n\0");
    printf("%s", ptr);


    if (mprotect(ptr, size, PROT_NONE) == -1) {
        perror("mprotect PROT_WRITE");
    }
    sleep(10);

    signal(SIGSEGV, sigsegv_handler);
    printf("%s", ptr);
}

void read_mmap_f() {
    signal(SIGSEGV, sigsegv_handler);

    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t size = 10 * page_size;

    char *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return;
    }
    printf("New region: %p\n", ptr);
    

    strcpy(ptr, "USELESS HELLO WORLD: GRAND FINALE");

    if (mprotect(ptr, page_size, PROT_NONE) == -1) {
        perror("mprotect PROT_READ");
    }
    sleep(10);

    ptr[0] = 'I';
}

void munmap_mmap_f() {
    signal(SIGSEGV, sigsegv_handler);

    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t size = 10 * page_size;

    char *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return;
    }
    printf("New region: %p\n", ptr);
    sleep(15);

    if (munmap(ptr + 4 * page_size, 3 * page_size) == -1) {
        perror("munmap 4...6");
    } else {
        printf("region munmap 4...6\n");
    }
}

int main() {
    printf("PID: %d\n", getpid());
    sleep(1);
    // stack_f(0);
    // heap_f();
    // write_mmap_f();
    read_mmap_f();
    // munmap_mmap_f();

    sleep(3000);
    return EXIT_SUCCESS;
}
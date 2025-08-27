#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>    


#define HEAP_SIZE (5 * 1024 * 1024)

void *heap_ptr = NULL;

typedef struct chunk {
    size_t size;
    struct chunk *next_chunk;
    struct chunk *prev_chunk;
    int isFree;
} chunk;

chunk *heap_chunks = NULL;

void initialize_my_heap(int fd) {
    if (fd == -1) {
        heap_ptr = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    } else {
        heap_ptr = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
    if (heap_ptr == MAP_FAILED) {
        perror("heap initialization (mmap)");
        if (fd != -1) {
            close(fd);
        }
        exit(EXIT_FAILURE);
    }

    heap_chunks = (chunk *) heap_ptr;
    heap_chunks->next_chunk = NULL;
    heap_chunks->prev_chunk = NULL;
    heap_chunks->size = HEAP_SIZE - sizeof(chunk);
    heap_chunks->isFree = 1;
    printf("%lu\n", sizeof(chunk));
}

void deinitialize_my_heap() {
    if (munmap(heap_ptr, HEAP_SIZE) == -1) {
        perror("heap deinitialization (munmap)");
        exit(EXIT_FAILURE);
    }
}
//atexit
void *my_malloc(size_t size) {
    chunk *current = heap_chunks;
    while (current != NULL) {
        if (current->isFree && (current->size >= size)) {
            if (current->size > size + sizeof(chunk)) {
                chunk *new_chunk = (chunk *) ((char *) current + sizeof(chunk) + size);
                new_chunk->isFree = 1;
                new_chunk->next_chunk = current->next_chunk;
                new_chunk->prev_chunk = current;
                new_chunk->size = current->size - size - sizeof(chunk);
            
                current->isFree = 0;
                current->next_chunk = new_chunk;
                current->size = size;
            } else {
                current->isFree = 0;
            }
            return (char *) current + sizeof(chunk); 
        }
        current = current->next_chunk;
    }
    return NULL;
}

void merge_free_chunks(chunk *ptr) {
    if ((ptr->next_chunk != NULL) && ptr->next_chunk->isFree) {
        chunk *next = ptr->next_chunk;
        ptr->size += sizeof(chunk) + next->size;
        ptr->next_chunk = next->next_chunk;
        if (ptr->next_chunk != NULL) {
            ptr->next_chunk->prev_chunk = ptr;
        }
    }
    if ((ptr->prev_chunk != NULL) && ptr->prev_chunk->isFree) {
        chunk *prev = ptr->prev_chunk;
        prev->size += sizeof(chunk) + ptr->size;
        prev->next_chunk = ptr->next_chunk;
        if (ptr->next_chunk != NULL) {
            prev->next_chunk->prev_chunk = prev;
        }
    }
}

void my_free(void *ptr) {
    if (!ptr) {
        return;
    }

    chunk *current_meta_ptr = ptr - sizeof(chunk);
    if (current_meta_ptr->next_chunk && (current_meta_ptr->next_chunk->prev_chunk != current_meta_ptr)) {
        write(2, "incorrect pointer to allocated space\n", 38);
        exit(EXIT_FAILURE);
    } else if (current_meta_ptr->prev_chunk && (current_meta_ptr->prev_chunk->next_chunk != current_meta_ptr)) {
        write(2, "incorrect pointer to allocated space\n", 38);
        exit(EXIT_FAILURE);
    }

    current_meta_ptr->isFree = 1;
    merge_free_chunks(current_meta_ptr);
}

int main() {
    printf("PID: %d\n", getpid());


    int fd = open("heap_dump.bin", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("open");
        exit(1);
    }   

    if (ftruncate(fd, HEAP_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(1);
    }


    initialize_my_heap(fd);
    char *ptr1 = (char *) my_malloc(14);
    if (ptr1) {
        strcpy(ptr1, "Hello, world!");
        fprintf(stdout, "ptr1: %s %p\n", ptr1, ptr1);
    }

    char *ptr2 = (char *) my_malloc(26);
    if (ptr2) {
        strcpy(ptr2, "Bye, my pretty nice world");
        fprintf(stdout, "ptr2: %s %p\n", ptr2, ptr2);
    }
    char *ptr3 = (char *) my_malloc(HEAP_SIZE);
    fprintf(stdout, "ptr3: %p \n", ptr3);
    my_free(ptr1);
    char *ptr4 = my_malloc(26);
    char *ptr5 = my_malloc(26);
    if (ptr4) {
        strcpy(ptr4, "Bye, my pretty nice world");
        fprintf(stdout, "ptr4: %s %p\n", ptr4, ptr4);
    }
    if (ptr5) {
        strcpy(ptr5, "Bye, my pretty nice world");
        fprintf(stdout, "ptr5: %s %p\n", ptr5, ptr5);
    }
    my_free(ptr2);
    my_free(ptr5);
    my_free(ptr4);
    deinitialize_my_heap();

    close(fd);
    return EXIT_SUCCESS;
}
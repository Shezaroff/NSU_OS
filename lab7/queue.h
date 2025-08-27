#include <stdlib.h>

#define BUFFER_SIZE 1024


typedef struct Node {
    struct Node* next;
    struct Node* prev;
    char buffer[BUFFER_SIZE];
    int size_buffer;
} Node;

typedef struct Queue {
    Node* head;
    Node* tail;
    int size;
} Queue;

Queue* init_queue() {
    Queue* new_queue = (Queue*) malloc(sizeof(Queue));
    if (new_queue != NULL) {
        new_queue->head = NULL;
        new_queue->tail = NULL;
        new_queue->size = 0;
        return new_queue;
    }
    return NULL;
}

int push_queue(Queue* queue) {
    if (queue == NULL) {
        return 1;
    }

    Node* new_node = (Node*) malloc(sizeof(Node));
    if (new_node == NULL) {
        return 1;
    }

    new_node->prev = NULL;
    new_node->buffer;
    new_node->size_buffer = 0;
    new_node->next = queue->head;
    if (queue->head != NULL) {
        queue->head->prev = new_node;
    }
    queue->head = new_node;
    if (queue->tail == NULL) {
        queue->tail = new_node;
    }
    queue->size += 1;
    return 0;
}

Node* pop_queue(Queue* queue) {
    if ((queue == NULL) || (queue->size == 0)) {
        return NULL;
    }

    Node* res = queue->tail;
    queue->tail = res->prev;
    if (queue->tail != NULL) {
        queue->tail->next = NULL;
    }
    queue->size -= 1;
    return res;
}

int get_size_queue(Queue* queue) {
    return queue->size;
}

void free_node_queue(Node* node) {
    if (node != NULL) {
        free_node_queue(node->next);
        free(node);
    }
}

void free_queue(Queue* queue) {
    if (queue != NULL) {
        free_node_queue(queue->head);
    }
    free(queue);
}
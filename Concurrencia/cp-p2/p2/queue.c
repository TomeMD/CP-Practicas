#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include "queue.h"

// circular array
typedef struct _queue {
    int size;
    int pos;
    int first;
    void **data;
    sem_t *used, *empty, *mutex;
} _queue;

queue q_create(int size) {
    queue q = malloc(sizeof(_queue));
    q->empty = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    q->used = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    q->mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    sem_init(q->empty, 1, size);
    sem_init(q->used, 1, 0);
    sem_init(q->mutex, 1, 1);
    q->size  = size;
    q->pos  = 0;
    q->first = 0;
    q->data  = malloc(size*sizeof(void *));
    
    return q;
}

int q_elements(queue q) {
    return q->pos;
}

int q_insert(queue q, void *elem) {
    
	sem_wait(q->empty);
	sem_wait(q->mutex);
    q->data[(q->first+q->pos) % q->size] = elem;    
    q->pos++;
    sem_post(q->mutex);
	sem_post(q->used);
    
    return 1;
}

void *q_remove(queue q) {
    void *res;
    sem_wait(q->used);
    sem_wait(q->mutex);
    res = q->data[q->first];
    q->first = (q->first+1) % q->size;
    q->pos--;
    sem_post(q->mutex);
	sem_post(q->empty);
    
    return res;
}

void q_destroy(queue q) {
	sem_destroy(q->empty);
	sem_destroy(q->used);
	sem_destroy(q->mutex);
    free(q->data);
    free(q);
}

#include "producer-consumer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int pcq_create(pc_queue_t *queue, size_t capacity){
    //allocate memory for buffer
    queue->pcq_buffer = (void **)malloc(sizeof(void*) * capacity);

    if (queue->pcq_buffer == NULL){
        return -1;
    }

    //initialization
    queue->pcq_capacity = capacity;
    queue->pcq_current_size = 0;
    queue->pcq_head = 0;
    queue->pcq_tail = 0;

    pthread_mutex_init(&queue->pcq_current_size_lock, NULL);
    pthread_mutex_init(&queue->pcq_head_lock, NULL);
    pthread_mutex_init(&queue->pcq_tail_lock, NULL);
    pthread_mutex_init(&queue->pcq_pusher_condvar_lock, NULL);
    pthread_mutex_init(&queue->pcq_popper_condvar_lock, NULL);

    return 0;
}

int pcq_destroy(pc_queue_t *queue){
    //release memory allocated for the buffer
    free(queue->pcq_buffer);
    queue->pcq_buffer = NULL;

    pthread_mutex_destroy(&queue->pcq_current_size_lock);
    pthread_mutex_destroy(&queue->pcq_head_lock);
    pthread_mutex_destroy(&queue->pcq_tail_lock);
    pthread_mutex_destroy(&queue->pcq_pusher_condvar_lock);
    pthread_mutex_destroy(&queue->pcq_popper_condvar_lock);

    queue->pcq_capacity = 0;
    queue->pcq_current_size = 0;
    queue->pcq_head = 0;
    queue->pcq_tail = 0;

    return 0;
}

int pcq_enqueue(pc_queue_t *queue, void *element){
    pthread_mutex_lock(&queue->pcq_current_size_lock);
    //if queue is full, waits until it is not
    while(queue->pcq_current_size == queue->pcq_capacity){
        pthread_cond_wait(&queue->pcq_pusher_condvar, &queue->pcq_current_size_lock);
    }
    pthread_mutex_unlock(&queue->pcq_current_size_lock);

    pthread_mutex_lock(&queue->pcq_head_lock);
    //if head reaches the end, goes to the begining of the buffer, like a circular buffer
    queue->pcq_head = (queue->pcq_head + queue->pcq_capacity -1) % queue->pcq_capacity;
    queue->pcq_buffer[queue->pcq_head] = element;
    pthread_mutex_unlock(&queue->pcq_tail_lock);

    pthread_mutex_lock(&queue->pcq_current_size_lock);
    queue->pcq_current_size++;
    pthread_cond_signal(&queue->pcq_popper_condvar);
    pthread_mutex_unlock(&queue->pcq_current_size_lock);

    return 0;
}

void* pc_dequeue(pc_queue_t *queue){
    void* result;

    pthread_mutex_lock(&queue->pcq_current_size_lock);
    //if the queue is empty, waits until it is not
    while (queue->pcq_current_size == 0){
        pthread_cond_wait(&queue->pcq_popper_condvar, &queue->pcq_current_size_lock);
    }
    pthread_mutex_unlock(&queue->pcq_current_size_lock);

    pthread_mutex_lock(&queue->pcq_tail_lock);
    result = queue->pcq_buffer[queue->pcq_tail];
    //if tail recahes the end, goes to the begining of the buffer, like a circular buffer
    queue->pcq_tail = (queue->pcq_tail + 1) % queue->pcq_capacity;
    pthread_mutex_unlock(&queue->pcq_tail_lock);
    
    pthread_mutex_lock(&queue->pcq_current_size_lock);
    queue->pcq_current_size--;
    pthread_cond_signal(&queue->pcq_pusher_condvar);
    pthread_mutex_unlock(&queue->pcq_current_size_lock);

    return result;
}
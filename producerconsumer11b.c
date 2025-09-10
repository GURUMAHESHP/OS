#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5   // size of buffer

int buffer[BUFFER_SIZE];
int in = 0, out = 0;   // indices for circular buffer

pthread_mutex_t mutex;
sem_t empty, full;

void *producer(void *arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        item = rand() % 100; // generate random item

        sem_wait(&empty);              // wait for empty slot
        pthread_mutex_lock(&mutex);    // lock buffer

        buffer[in] = item;
        printf("Producer produced %d at position %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);  // unlock buffer
        sem_post(&full);               // signal full slot

        sleep(1); // simulate time
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        sem_wait(&full);               // wait for full slot
        pthread_mutex_lock(&mutex);    // lock buffer

        item = buffer[out];
        printf("Consumer consumed %d from position %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);  // unlock buffer
        sem_post(&empty);              // signal empty slot

        sleep(2); // simulate time
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod, cons;

    // Initialize mutex and semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE); // all slots empty initially
    sem_init(&full, 0, 0);           // no items initially

    // Create producer and consumer threads
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    // Wait for both threads
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // Cleanup
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
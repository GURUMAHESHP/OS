#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t wrt;               // semaphore to control writer access
pthread_mutex_t mutex;   // mutex for protecting read_count
int read_count = 0;      // number of readers currently reading
int shared_data = 0;     // shared resource (example: an integer)

void *reader(void *arg) {
    int id = *((int*)arg);
    while (1) {
        pthread_mutex_lock(&mutex);
        read_count++;
        if (read_count == 1) {
            sem_wait(&wrt); // first reader blocks writers
        }
        pthread_mutex_unlock(&mutex);

        // ---- Critical Section ----
        printf("Reader %d: read shared_data = %d\n", id, shared_data);
        sleep(1);
        // --------------------------

        pthread_mutex_lock(&mutex);
        read_count--;
        if (read_count == 0) {
            sem_post(&wrt); // last reader allows writers
        }
        pthread_mutex_unlock(&mutex);

        sleep(1); // simulate time
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *((int*)arg);
    while (1) {
        sem_wait(&wrt); // wait for exclusive access

        // ---- Critical Section ----
        shared_data += 1; // writer updates data
        printf("Writer %d: wrote shared_data = %d\n", id, shared_data);
        sleep(1);
        // --------------------------

        sem_post(&wrt); // release access
        sleep(2);       // simulate time
    }
    return NULL;
}

int main() {
    pthread_t rtid[3], wtid[2];
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {1, 2};

    // Initialize
    sem_init(&wrt, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    // Create reader threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&rtid[i], NULL, reader, &reader_ids[i]);
    }

    // Create writer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&wtid[i], NULL, writer, &writer_ids[i]);
    }

    // Join (never ends in this demo)
    for (int i = 0; i < 3; i++) pthread_join(rtid[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(wtid[i], NULL);

    // Cleanup
    sem_destroy(&wrt);
    pthread_mutex_destroy(&mutex);

    return 0;
}
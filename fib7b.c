#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

// Function to generate Fibonacci series
void generateFibonacci(int n, int *shm_array) {
    if (n >= 1) shm_array[0] = 0;
    if (n >= 2) shm_array[1] = 1;

    for (int i = 2; i < n; i++) {
        shm_array[i] = shm_array[i - 1] + shm_array[i - 2];
    }
}

int main() {
    int n;
    key_t key = 1234;   // fixed key for identification
    printf("[Parent] Enter number of Fibonacci terms: ");
    scanf("%d", &n);

    // Create shared memory
    int shmid = shmget(key, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    printf("[Parent] Created shared memory with key=%d, id=%d\n", key, shmid);

    // Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        // ===== Child process =====
        int *shm_array = (int*) shmat(shmid, NULL, 0);
        if (shm_array == (int*) -1) {
            perror("shmat (child)");
            exit(1);
        }
        printf("[Child] Attached to shared memory with key=%d, id=%d, address=%p\n", key, shmid, shm_array);

        // Generate Fibonacci into shared memory
        generateFibonacci(n, shm_array);
        printf("[Child] Finished generating first %d Fibonacci numbers\n", n);

        // Detach shared memory in child
        shmdt(shm_array);
        exit(0);
    }
    else {
        // ===== Parent process =====
        wait(NULL);  // wait for child to finish
        printf("[Parent] Child finished executing.\n");

        int *shm_array = (int*) shmat(shmid, NULL, 0);
        if (shm_array == (int*) -1) {
            perror("shmat (parent)");
            exit(1);
        }
        printf("[Parent] Attached to shared memory with key=%d, id=%d, address=%p\n", key, shmid, shm_array);

        // Print Fibonacci series
        printf("[Parent] The first %d Fibonacci numbers are:\n", n);
        for (int i = 0; i < n; i++) {
            printf("%d ", shm_array[i]);
        }
        printf("\n");

        // Detach and remove shared memory
        shmdt(shm_array);
        shmctl(shmid, IPC_RMID, NULL);
        printf("[Parent] Shared memory deleted.\n");
    }

    return 0;
}
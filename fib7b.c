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
    printf("Enter number of Fibonacci terms: ");
    scanf("%d", &n);

    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        // Child process
        int *shm_array = (int*) shmat(shmid, NULL, 0);
        if (shm_array == (int*) -1) {
            perror("shmat (child)");
            exit(1);
        }

        // Generate Fibonacci into shared memory
        generateFibonacci(n, shm_array);

        // Detach shared memory in child
        shmdt(shm_array);

        exit(0);
    }
    else {
        // Parent process
        wait(NULL);  // wait for child to finish

        int *shm_array = (int*) shmat(shmid, NULL, 0);
        if (shm_array == (int*) -1) {
            perror("shmat (parent)");
            exit(1);
        }

        // Print Fibonacci series
        printf("Fibonacci Series: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", shm_array[i]);
        }
        printf("\n");

        // Detach and remove shared memory
        shmdt(shm_array);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
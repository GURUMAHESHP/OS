#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int n;
    printf("Enter number of odd terms: ");
    scanf("%d", &n);

    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // ===== Child Process =====
        int *shm_array = (int *) shmat(shmid, NULL, 0);
        if (shm_array == (void *)-1) {
            perror("shmat (child)");
            exit(1);
        }

        // Generate N odd numbers
        for (int i = 0; i < n; i++) {
            shm_array[i] = 2 * i + 1;
        }

        // Detach
        shmdt(shm_array);
        exit(0);

    } else {
        // ===== Parent Process =====
        wait(NULL); // wait for child

        int *shm_array = (int *) shmat(shmid, NULL, 0);
        if (shm_array == (void *)-1) {
            perror("shmat (parent)");
            exit(1);
        }

        // Print odd numbers
        printf("First %d odd numbers: ", n);
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
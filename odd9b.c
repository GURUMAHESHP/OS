#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int n;
    key_t key = 1234;  // fixed key (for printing like your output)
    printf("[Parent] Enter n: ");
    scanf("%d", &n);

    // Create shared memory
    int shmid = shmget(key, n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    printf("[Parent] Created shared memory with key=%d, id=%d\n", key, shmid);

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
        printf("[Child] Attached to shared memory with key=%d, id=%d, address=%p\n", key, shmid, shm_array);

        // Generate N odd numbers
        for (int i = 0; i < n; i++) {
            shm_array[i] = 2 * i + 1;
        }
        printf("[Child] Finished generating first %d odd numbers\n", n);

        // Detach
        shmdt(shm_array);
        exit(0);

    } else {
        // ===== Parent Process =====
        wait(NULL); // wait for child
        printf("[Parent] Child finished executing.\n");

        int *shm_array = (int *) shmat(shmid, NULL, 0);
        if (shm_array == (void *)-1) {
            perror("shmat (parent)");
            exit(1);
        }
        printf("[Parent] The first %d odd numbers are:\n", n);

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
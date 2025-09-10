#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Function to check prime
int isPrime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "[Parent] Usage: %s <M> <N>\n", argv[0]);
        exit(1);
    }

    int M = atoi(argv[1]);
    int N = atoi(argv[2]);

    if (M > N) {
        fprintf(stderr, "[Parent] Error: M should be <= N\n");
        exit(1);
    }

    const char *shm_name = "/prime_shm";
    int shm_fd;
    int *shm_array;

    // Create shared memory
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Rough upper bound on number of primes
    int max_size = (N - M + 1);
    if (ftruncate(shm_fd, (max_size + 1) * sizeof(int)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    printf("[Parent] Created shared memory object '%s'\n", shm_name);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // ===== Child Process =====
        shm_array = mmap(0, (max_size + 1) * sizeof(int),
                         PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_array == MAP_FAILED) {
            perror("mmap (child)");
            exit(1);
        }

        printf("[Child] Generating primes between %d and %d...\n", M, N);

        int count = 0;
        for (int i = M; i <= N; i++) {
            if (isPrime(i)) {
                shm_array[count++] = i;
            }
        }

        // Store count at last index
        shm_array[max_size] = count;

        printf("[Child] Finished generating %d primes.\n", count);

        munmap(shm_array, (max_size + 1) * sizeof(int));
        close(shm_fd);
        exit(0);

    } else {
        // ===== Parent Process =====
        wait(NULL);

        shm_array = mmap(0, (max_size + 1) * sizeof(int),
                         PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_array == MAP_FAILED) {
            perror("mmap (parent)");
            exit(1);
        }

        int count = shm_array[max_size];
        printf("[Parent] Child finished execution.\n");
        printf("[Parent] Prime numbers between %d and %d are:\n", M, N);

        for (int i = 0; i < count; i++) {
            printf("%d ", shm_array[i]);
        }
        printf("\n");

        munmap(shm_array, (max_size + 1) * sizeof(int));
        close(shm_fd);
        shm_unlink(shm_name);
        printf("[Parent] Shared memory deleted.\n");
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main() {
    int n;
    printf("Enter number of Fibonacci terms: ");
    scanf("%d", &n);

    const char *shm_name = "/fib_shm"; // shared memory name
    int shm_fd;
    int *shm_array;

    // Create shared memory
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, n * sizeof(int)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Fork child
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // ===== Child Process =====
        shm_array = mmap(0, n * sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_array == MAP_FAILED) {
            perror("mmap (child)");
            exit(1);
        }

        // Generate Fibonacci
        if (n > 0) shm_array[0] = 0;
        if (n > 1) shm_array[1] = 1;
        for (int i = 2; i < n; i++) {
            shm_array[i] = shm_array[i - 1] + shm_array[i - 2];
        }

        // Unmap in child
        munmap(shm_array, n * sizeof(int));
        close(shm_fd);
        exit(0);

    } else {
        // ===== Parent Process =====
        wait(NULL); // wait for child to finish

        shm_array = mmap(0, n * sizeof(int), PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_array == MAP_FAILED) {
            perror("mmap (parent)");
            exit(1);
        }

        // Print Fibonacci
        printf("Fibonacci Series: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", shm_array[i]);
        }
        printf("\n");

        // Cleanup
        munmap(shm_array, n * sizeof(int));
        close(shm_fd);
        shm_unlink(shm_name); // remove shared memory
    }

    return 0;
}
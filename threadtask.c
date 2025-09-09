#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Function to calculate sum of first n natural numbers
void* sum_n(void* arg) {
    int n = *((int*)arg);
    int sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += i;
    }
    printf("Sum of first %d natural numbers = %d\n", n, sum);
    pthread_exit(NULL); // terminate thread
}

// Function to calculate factorial of n
void* factorial_n(void* arg) {
    int n = *((int*)arg);
    long long fact = 1;
    for (int i = 1; i <= n; i++) {
        fact *= i;
    }
    printf("Factorial of %d = %lld\n", n, fact);
    pthread_exit(NULL); // terminate thread
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]); // convert command line argument to int

    pthread_t t1, t2;

    // Create two threads
    pthread_create(&t1, NULL, sum_n, &n);
    pthread_create(&t2, NULL, factorial_n, &n);

    // Wait for both threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Main thread exiting...\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Функция для факторизации числа
void factorize(long long number) {
    for (long long i = 1; i * i <= number; i++) {
        if (number % i == 0) {
            // Печатаем множители числа
            printf("%lld ", i);
            if (i != number / i) {
                printf("%lld ", number / i);
            }
        }
    }
}

// Функция для выполнения benchmark'а факторизации
void factorize_benchmark(int iterations) {
    printf("Starting factorization benchmark...\n");

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    for (int i = 0; i < iterations; i++) {
        struct timespec iter_start, iter_end;
        clock_gettime(CLOCK_REALTIME, &iter_start);

        // Число для факторизации
        long long number = 1000000000LL + i;
        printf("Iteration %d: Factors of %lld: ", i + 1, number);
        factorize(number);
        printf("\n");

        clock_gettime(CLOCK_REALTIME, &iter_end);
        double iter_time = (iter_end.tv_sec - iter_start.tv_sec) +
                           (iter_end.tv_nsec - iter_start.tv_nsec) / 1e9;
        printf("Time for iteration %d: %.3f seconds\n", i + 1, iter_time);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double total_time = (end_time.tv_sec - start_time.tv_sec) +
                        (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Benchmark completed in %.3f seconds.\n", total_time);
}

// Основная функция
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <iterations>\n", argv[0]);
        return 1;
    }

    int iterations = atoi(argv[1]);
    if (iterations <= 0) {
        fprintf(stderr, "Error: iterations must be a positive integer.\n");
        return 1;
    }

    factorize_benchmark(iterations);
    return 0;
}

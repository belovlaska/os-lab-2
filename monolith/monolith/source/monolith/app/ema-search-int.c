#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define MAX_ARRAY_SIZE 67108864
#define CHUNK_SIZE 8388608

void initialize_data_file(const char* filename) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, MAX_ARRAY_SIZE * sizeof(int));

    for (int i = 0; i < MAX_ARRAY_SIZE; i += CHUNK_SIZE) {
        int* chunk = (int*)mmap(
                NULL, CHUNK_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, i * sizeof(int)
        );
        if (chunk == MAP_FAILED) {
            perror("Failed to mmap file");
            close(fd);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < CHUNK_SIZE && (i + j) < MAX_ARRAY_SIZE; j++) {
            chunk[j] = rand() % 1000000;
        }

        msync(chunk, CHUNK_SIZE * sizeof(int), MS_SYNC);
        munmap(chunk, CHUNK_SIZE * sizeof(int));
    }

    close(fd);
}

void ema_search_int(const char* filename, int iterations) {
    printf("Starting EMA search benchmark...\n");

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return;
    }

    for (int iter = 0; iter < iterations; iter++) {
        int target = rand() % 1000000;
        int found_index = -1;

        for (int chunk_start = 0; chunk_start < MAX_ARRAY_SIZE; chunk_start += CHUNK_SIZE) {
            int chunk_end = chunk_start + CHUNK_SIZE;
            if (chunk_end > MAX_ARRAY_SIZE) {
                chunk_end = MAX_ARRAY_SIZE;
            }

            int* chunk = (int*)mmap(
                    NULL, CHUNK_SIZE * sizeof(int), PROT_READ, MAP_SHARED, fd, chunk_start * sizeof(int)
            );
            if (chunk == MAP_FAILED) {
                perror("Failed to mmap file");
                close(fd);
                return;
            }

            for (int i = 0; i < chunk_end - chunk_start; i++) {
                if (chunk[i] == target) {
                    found_index = chunk_start + i;
                    break;
                }
            }

            munmap(chunk, CHUNK_SIZE * sizeof(int));
            if (found_index != -1)
                break;
        }

        printf("Iteration %d: Target: %d, Found at index: %d\n", iter + 1, target, found_index);
    }

    close(fd);
    clock_gettime(CLOCK_REALTIME, &end_time);
    double elapsed_time =
            (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("EMA search completed in %.3f seconds.\n", elapsed_time);
}

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

    const char* filename = "ema_data.bin";

    // Генерируем файл с данными, если он не существует
    if (access(filename, F_OK) == -1) {
        printf("Data file not found. Initializing...\n");
        initialize_data_file(filename);
    }

    // Запуск поиска
    ema_search_int(filename, iterations);

    return 0;
}

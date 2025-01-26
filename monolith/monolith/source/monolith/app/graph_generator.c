#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define NUM_VERTICES 100  // Количество вершин в графе
#define MAX_VALUE 1000    // Максимальное значение вершины
#define NEIGHBORS 4       // Количество соседей

// Структура вершины
typedef struct {
    int value;               // Значение вершины
    uint32_t neighbors[NEIGHBORS];  // Смещения соседей
} Vertex;

void generate_graph(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create graph file");
        exit(1);
    }

    // Инициализация случайного числа
    srand(time(NULL));

    // Массив для хранения смещений соседей
    uint32_t offsets[NUM_VERTICES];

    // Предварительное вычисление смещений для всех вершин
    for (int i = 0; i < NUM_VERTICES; i++) {
        offsets[i] = i * sizeof(Vertex);
    }

    // Генерация вершин
    for (int i = 0; i < NUM_VERTICES; i++) {
        Vertex vertex;
        vertex.value = rand() % MAX_VALUE;  // Случайное значение вершины

        // Генерация случайных соседей
        for (int j = 0; j < NEIGHBORS; j++) {
            vertex.neighbors[j] = offsets[rand() % NUM_VERTICES];
        }

        // Запись вершины в файл
        fwrite(&vertex, sizeof(Vertex), 1, file);
    }

    fclose(file);
    printf("Graph generated and saved to %s\n", filename);
}

int main() {
    const char *filename = "graph.bin";
    generate_graph(filename);
    return 0;
}

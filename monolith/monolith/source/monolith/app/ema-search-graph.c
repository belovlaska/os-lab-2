#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define CACHE_SIZE 128
#define MAX_NEIGHBORS 4

typedef struct {
    int value;
    int neighbors[MAX_NEIGHBORS];
} Vertex;

typedef struct CacheEntry {
    int offset;               // Смещение вершины в файле
    Vertex vertex;            // Данные вершины
    struct CacheEntry *prev;  // Предыдущий элемент в LRU
    struct CacheEntry *next;  // Следующий элемент в LRU
} CacheEntry;

typedef struct {
    CacheEntry *head;  // Голова списка (самая новая запись)
    CacheEntry *tail;  // Хвост списка (самая старая запись)
    int size;          // Текущий размер кэша
    CacheEntry entries[CACHE_SIZE];
} LRUCache;

void init_cache(LRUCache *cache) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    memset(cache->entries, 0, sizeof(cache->entries));
}

Vertex *get_vertex(LRUCache *cache, int fd, int offset) {
    // Проверяем, есть ли вершина в кэше
    for (CacheEntry *entry = cache->head; entry != NULL; entry = entry->next) {
        if (entry->offset == offset) {
            // Перемещаем вершину в начало списка
            if (entry != cache->head) {
                if (entry == cache->tail) {
                    cache->tail = entry->prev;
                    cache->tail->next = NULL;
                } else {
                    entry->prev->next = entry->next;
                    entry->next->prev = entry->prev;
                }
                entry->prev = NULL;
                entry->next = cache->head;
                cache->head->prev = entry;
                cache->head = entry;
            }
            return &entry->vertex;
        }
    }

    // Если вершины нет в кэше, загружаем с диска
    if (cache->size < CACHE_SIZE) {
        CacheEntry *entry = &cache->entries[cache->size++];
        entry->offset = offset;
        pread(fd, &entry->vertex, sizeof(Vertex), offset);
        entry->prev = NULL;
        entry->next = cache->head;
        if (cache->head) cache->head->prev = entry;
        cache->head = entry;
        if (!cache->tail) cache->tail = entry;
        return &entry->vertex;
    }

    // Если кэш заполнен, вытесняем старую запись
    CacheEntry *entry = cache->tail;
    cache->tail = entry->prev;
    cache->tail->next = NULL;
    entry->offset = offset;
    pread(fd, &entry->vertex, sizeof(Vertex), offset);
    entry->prev = NULL;
    entry->next = cache->head;
    cache->head->prev = entry;
    cache->head = entry;
    return &entry->vertex;
}
int search_graph(LRUCache *cache, int fd, int start_offset, int target_value) {
    int visited[CACHE_SIZE] = {0};
    int queue[CACHE_SIZE];
    int front = 0, back = 0;

    queue[back++] = start_offset;
    visited[start_offset] = 1;

    while (front < back) {
        int offset = queue[front++];
        Vertex *vertex = get_vertex(cache, fd, offset);
        if (vertex->value == target_value) {
            return offset;
        }

        for (int i = 0; i < MAX_NEIGHBORS; i++) {
            int neighbor_offset = vertex->neighbors[i];
            if (neighbor_offset != -1 && !visited[neighbor_offset]) {
                visited[neighbor_offset] = 1;
                queue[back++] = neighbor_offset;
            }
        }
    }

    return -1;  // Не найдено
}
int main(int argc, char* argv[]) {
    int fd = open("graph.bin", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open graph file");
        return 1;
    }

    LRUCache cache;
    init_cache(&cache);


    int start_offset = atoi(argv[2]);
    int target_value = atoi(argv[3]);

    int result_offset = search_graph(&cache, fd, start_offset, target_value);
    if (result_offset != -1) {
        printf("Found target value at offset %d\n", result_offset);
    } else {
        printf("Target value not found\n");
    }

    close(fd);
    return 0;
}

#include <stdio.h>
#include <stdint.h>

#define NEIGHBORS 4

typedef struct {
    int value;
    uint32_t neighbors[NEIGHBORS];
} Vertex;

void print_graph(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open graph file");
        return;
    }

    Vertex vertex;
    int index = 0;

    printf("Graph contents:\n");
    while (fread(&vertex, sizeof(Vertex), 1, file)) {
        printf("Vertex %d: Value=%d, Neighbors=[%u, %u, %u, %u]\n",
               index, vertex.value, vertex.neighbors[0], vertex.neighbors[1],
               vertex.neighbors[2], vertex.neighbors[3]);
        index++;
    }

    fclose(file);
}

int main() {
    const char *filename = "graph.bin";
    print_graph(filename);
    return 0;
}

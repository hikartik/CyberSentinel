// heuristic.c
#include "utils.h"
#include "heuristic.h"
#include <math.h>

int analyze_file_entropy(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("fopen");
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size <= 0) {
        fclose(file);
        return 0;
    }
    unsigned char *buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return 0;
    }
    fread(buffer, 1, size, file);
    fclose(file);

    double entropy = 0.0;
    int freq[256] = {0};
    for (long i = 0; i < size; i++) {
        freq[buffer[i]]++;
    }
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            double p = (double)freq[i] / size;
            entropy -= p * log2(p);
        }
    }
    free(buffer);
    printf("[Heuristic] Entropy for %s: %.2f\n", filepath, entropy);
    // For demo purposes, files with entropy > 7.5 are flagged.
    if (entropy > 7.5) {
        printf("[Heuristic] File %s flagged as suspicious due to high entropy.\n", filepath);
        return 1;
    }
    return 0;
}


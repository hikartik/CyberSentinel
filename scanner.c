// scanner.c
#include "utils.h"
#include "scanner.h"
#include "update.h"
#include <openssl/sha.h>

// Compute the SHA-256 hash of the file at filepath.
void compute_sha256(const char *filepath, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("fopen");
        return;
    }
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    unsigned char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);
    fclose(file);
}

// Convert a binary hash to a hex string.
void hash_to_hex(unsigned char hash[SHA256_DIGEST_LENGTH], char *output) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = 0;
}

// Compare the computed hash with the online signature database.
int scan_file_signature(const char *filepath) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    compute_sha256(filepath, hash);
    char hash_hex[SHA256_DIGEST_LENGTH * 2 + 1];
    hash_to_hex(hash, hash_hex);
    printf("[Scanner] Computed SHA-256 for %s: %s\n", filepath, hash_hex);

    size_t num_signatures = 0;
    char **signature_db = get_signature_db(&num_signatures);
    for (size_t i = 0; i < num_signatures; i++) {
        if (strcmp(hash_hex, signature_db[i]) == 0) {
            printf("[Scanner] File %s is infected! Matched signature: %s\n", filepath, signature_db[i]);
            return 1;
        }
    }
    printf("[Scanner] File %s appears clean.\n", filepath);
    return 0;
}


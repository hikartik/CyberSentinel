// update.c
#include "utils.h"
#include "update.h"
#include <curl/curl.h>
#include <unistd.h>
#include <ctype.h>

// Global signature database.
static char **signature_db = NULL;
static size_t num_signatures = 0;

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

void update_signatures(void) {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    if (curl_handle) {
        // Open-source malware hash feed (SHA-256 hashes with descriptions)
        curl_easy_setopt(curl_handle, CURLOPT_URL,
                         "https://raw.githubusercontent.com/Neo23x0/signature-base/master/iocs/otx-hash-iocs.txt");
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            fprintf(stderr, "[Update] curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            // Fallback to a minimal built-in signature
            const char *dummy_text = "d41d8cd98f00b204e9800998ecf8427e;dummy";
            free(chunk.memory);
            chunk.memory = strdup(dummy_text);
            chunk.size = strlen(dummy_text);
        }
        printf("[Update] Received signature data (%zu bytes)\n", chunk.size);

        // Free previous database if any
        for (size_t i = 0; i < num_signatures; i++) {
            free(signature_db[i]);
        }
        free(signature_db);
        signature_db = NULL;
        num_signatures = 0;

        // Parse lines of the feed: "HASH;description"
        char *saveptr = NULL;
        char *line = strtok_r(chunk.memory, "\n", &saveptr);
        while (line) {
            char *semi = strchr(line, ';');
            if (semi) {
                *semi = '\0';
            }
            size_t len = strlen(line);
            if (len == 64) {
                for (size_t i = 0; i < len; i++) {
                    line[i] = tolower((unsigned char)line[i]);
                }
                signature_db = realloc(signature_db, (num_signatures + 1) * sizeof(char *));
                signature_db[num_signatures] = strdup(line);
                printf("[Update] Loaded signature: %s\n", line);
                num_signatures++;
            }
            line = strtok_r(NULL, "\n", &saveptr);
        }
        if (num_signatures == 0) {
            fprintf(stderr, "[Update] No valid signatures loaded.\n");
        }
        curl_easy_cleanup(curl_handle);
    }
    curl_global_cleanup();
    free(chunk.memory);
}

char **get_signature_db(size_t *count) {
    *count = num_signatures;
    return signature_db;
}


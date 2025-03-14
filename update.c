// update.c
#include "utils.h"
#include "update.h"
#include <curl/curl.h>
#include "cJSON.h"
#include <unistd.h>

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
        // Dummy URL for the signature database.
        curl_easy_setopt(curl_handle, CURLOPT_URL, "http://example.com/signatures.json");
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl_handle);
        if (res != CURLE_OK) {
            fprintf(stderr, "[Update] curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            // Use a dummy JSON if download fails.
            const char *dummy_json = "{\"signatures\": [\"dummyhash1234567890abcdefdummyhash1234567890abcdef\"]}";
            free(chunk.memory);
            chunk.memory = strdup(dummy_json);
            chunk.size = strlen(dummy_json);
        }
        printf("[Update] Received signature data:\n%s\n", chunk.memory);
        cJSON *json = cJSON_Parse(chunk.memory);
        if (json == NULL) {
            fprintf(stderr, "[Update] Failed to parse JSON.\n");
        } else {
            cJSON *signatures = cJSON_GetObjectItemCaseSensitive(json, "signatures");
            if (cJSON_IsArray(signatures)) {
                num_signatures = cJSON_GetArraySize(signatures);
                signature_db = malloc(num_signatures * sizeof(char *));
                for (size_t i = 0; i < num_signatures; i++) {
                    cJSON *sig = cJSON_GetArrayItem(signatures, i);
                    if (cJSON_IsString(sig) && sig->valuestring != NULL) {
                        signature_db[i] = strdup(sig->valuestring);
                        printf("[Update] Loaded signature: %s\n", signature_db[i]);
                    }
                }
            }
            cJSON_Delete(json);
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


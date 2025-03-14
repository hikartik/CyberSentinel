// quarantine.c
#include "utils.h"
#include "quarantine.h"
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

void quarantine_file(const char *filepath, const char *quarantine_dir) {
    mkdir(quarantine_dir, 0700);
    char *file_copy = strdup(filepath);
    char *base = basename(file_copy);
    char target_path[512];
    snprintf(target_path, sizeof(target_path), "%s/%s", quarantine_dir, base);
    if (rename(filepath, target_path) == 0) {
        printf("[Quarantine] Moved %s to quarantine directory (%s).\n", filepath, target_path);
    } else {
        perror("[Quarantine] rename");
    }
    free(file_copy);
}


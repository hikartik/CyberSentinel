// process_monitor.c
#include "utils.h"
#include "process_monitor.h"
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// Helper to read a single line from a proc file
static int read_proc_line(const char *path, char *buf, size_t buflen) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    if (!fgets(buf, buflen, f)) {
        fclose(f);
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    fclose(f);
    return 1;
}

// Lists running processes with their names.
void list_processes(void) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    printf("[Process Monitor] Listing processes:\n");
    while ((entry = readdir(dir)) != NULL) {
        int numeric = 1;
        for (char *p = entry->d_name; *p; p++) {
            if (!isdigit(*p)) { numeric = 0; break; }
        }
        if (numeric) {
            char comm_path[256];
            snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);
            char name[256] = {0};
            if (read_proc_line(comm_path, name, sizeof(name))) {
                printf("  PID: %s  Name: %s\n", entry->d_name, name);
            }
        }
    }
    closedir(dir);
}

// Check if a process with the given name exhibits suspicious traits.
// Currently flags processes running from temporary directories.
int check_suspicious_process(const char *process_name) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return 0;
    }
    struct dirent *entry;
    int suspicious = 0;
    while ((entry = readdir(dir)) != NULL) {
        int numeric = 1;
        for (char *p = entry->d_name; *p; p++) {
            if (!isdigit(*p)) { numeric = 0; break; }
        }
        if (!numeric) continue;

        char comm_path[256];
        snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);
        char name[256] = {0};
        if (!read_proc_line(comm_path, name, sizeof(name)))
            continue;
        if (strcmp(name, process_name) != 0)
            continue;

        char exe_path[256];
        snprintf(exe_path, sizeof(exe_path), "/proc/%s/exe", entry->d_name);
        char resolved[512] = {0};
        ssize_t len = readlink(exe_path, resolved, sizeof(resolved) - 1);
        if (len >= 0) {
            resolved[len] = '\0';
            if (strncmp(resolved, "/tmp", 4) == 0 ||
                strncmp(resolved, "/var/tmp", 8) == 0 ||
                strncmp(resolved, "/dev/shm", 8) == 0) {
                printf("[Process Monitor] Process %s (PID %s) running from suspicious directory: %s\n",
                       name, entry->d_name, resolved);
                suspicious = 1;
            }
        } else {
            // If the executable path cannot be resolved, treat it as suspicious
            printf("[Process Monitor] Unable to resolve executable for PID %s.\n", entry->d_name);
            suspicious = 1;
        }
    }
    closedir(dir);

    if (suspicious) {
        printf("[Process Monitor] Suspicious process detected: %s\n", process_name);
    } else {
        printf("[Process Monitor] No suspicious instances of %s found.\n", process_name);
    }
    return suspicious;
}


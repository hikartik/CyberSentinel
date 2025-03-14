// process_monitor.c
#include "utils.h"
#include "process_monitor.h"
#include <dirent.h>
#include <ctype.h>

void list_processes(void) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return;
    }
    struct dirent *entry;
    printf("[Process Monitor] Listing processes:\n");
    while ((entry = readdir(dir)) != NULL) {
        int is_number = 1;
        for (char *p = entry->d_name; *p; p++) {
            if (!isdigit(*p)) { is_number = 0; break; }
        }
        if (is_number) {
            printf("  PID: %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

int check_suspicious_process(const char *process_name) {
    // For demonstration, flag if the process name contains "mal".
    if (strstr(process_name, "mal") != NULL) {
        printf("[Process Monitor] Process %s flagged as suspicious.\n", process_name);
        return 1;
    }
    return 0;
}

